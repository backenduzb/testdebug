#!/usr/bin/env python3
"""Local, API-key-free vision gateway for authorized practice screenshots.

It calls already-authenticated local Codex/OpenCode CLIs. The HTTP server binds
only to localhost and never logs image data or agent output.
"""
from __future__ import annotations

import base64
import json
import os
import re
import subprocess
import tempfile
import threading
import time
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

HOST = "127.0.0.1"
PORT = int(os.environ.get("LOCAL_AI_PORT", "8765"))
TIMEOUT = int(os.environ.get("LOCAL_AI_TIMEOUT", "120"))
MAX_IMAGE_BYTES = 8 * 1024 * 1024
RETRIES = 2

PROMPT = """Analyze the attached image from an explicitly authorized practice interface.
Identify the question and visible answer choices. Select the best-supported answer
only when the image is sufficient. Return JSON only, exactly with this shape:
{"questionId":"optional","selectedLabel":"A","status":"success","confidence":0.0}
status must be success, ambiguous, unreadable, unsupported, or error. Do not include
reasoning. If unreadable or incomplete, do not invent an answer.
"""

AGENTS = ["codex", "opencode"]


class AgentError(RuntimeError):
    pass


def _limit_error(text: str) -> bool:
    lowered = text.lower()
    return any(token in lowered for token in ("rate limit", "quota", "too many requests", "usage limit", "limit reached", "credits"))


def _extract_json(text: str) -> dict:
    text = text.strip()
    candidates = [text]
    candidates.extend(re.findall(r"\{.*?\}", text, flags=re.DOTALL))
    for candidate in reversed(candidates):
        try:
            value = json.loads(candidate)
            if isinstance(value, dict):
                return value
        except json.JSONDecodeError:
            continue
    return {"status": "error", "selectedLabel": "", "confidence": 0.0, "error": "Agent JSON qaytarmadi"}


def _run_agent(name: str, image_path: str, prompt: str) -> dict:
    if name == "codex":
        argv = ["codex", "exec", "--skip-git-repo-check", "--ephemeral", "--image", image_path, "--color", "never", prompt]
    else:
        argv = ["opencode", "run", "--file", image_path, "--format", "default", "--log-level", "ERROR", prompt]
    try:
        completed = subprocess.run(argv, text=True, capture_output=True, timeout=TIMEOUT, check=False)
    except FileNotFoundError as exc:
        raise AgentError(f"{name} CLI topilmadi: {exc}") from exc
    except subprocess.TimeoutExpired as exc:
        raise AgentError(f"{name} timeout ({TIMEOUT}s)") from exc
    output = (completed.stdout or "") + "\n" + (completed.stderr or "")
    if completed.returncode != 0:
        kind = "limit" if _limit_error(output) else "process"
        raise AgentError(f"{name} {kind} xatosi")
    result = _extract_json(output)
    result.setdefault("agent", name)
    return result


class Router:
    def __init__(self) -> None:
        self.lock = threading.Lock()
        self.cooldown_until = {name: 0.0 for name in AGENTS}
        self.index = 0

    def analyze(self, image: bytes, context: str, question_id: str) -> dict:
        if not image:
            return {"status": "error", "error": "Bo'sh image"}
        if len(image) > MAX_IMAGE_BYTES:
            return {"status": "error", "error": "Image 8 MB dan katta"}
        prompt = PROMPT + (f"\nQuestion id: {question_id}" if question_id else "") + (f"\nContext: {context}" if context else "")
        with self.lock:
            with tempfile.NamedTemporaryFile(suffix=".png") as image_file:
                image_file.write(image)
                image_file.flush()
                errors = []
                order = AGENTS[self.index:] + AGENTS[:self.index]
                for name in order:
                    if time.monotonic() < self.cooldown_until[name]:
                        continue
                    for attempt in range(RETRIES):
                        try:
                            result = _run_agent(name, image_file.name, prompt)
                            if result.get("status") in {"success", "ambiguous", "unreadable", "unsupported"}:
                                self.index = AGENTS.index(name)
                                return result
                            raise AgentError(result.get("error", "Agent error status"))
                        except AgentError as exc:
                            errors.append(f"{name}: {exc}")
                            if "limit" in str(exc).lower():
                                self.cooldown_until[name] = time.monotonic() + 3600
                                break
                            if attempt + 1 < RETRIES:
                                time.sleep(2 ** attempt)
                return {"status": "error", "error": "; ".join(errors) or "Available agent yo'q"}


router = Router()


class Handler(BaseHTTPRequestHandler):
    def log_message(self, *_args) -> None:
        return

    def do_GET(self) -> None:
        if self.path == "/health":
            self._send(200, {"ok": True, "agents": AGENTS})
        else:
            self._send(404, {"error": "not found"})

    def do_POST(self) -> None:
        if self.path != "/v1/analyze":
            self._send(404, {"error": "not found"})
            return
        try:
            length = int(self.headers.get("Content-Length", "0"))
            body = json.loads(self.rfile.read(length))
            image = base64.b64decode(body.get("imageBase64", ""), validate=True)
            result = router.analyze(image, str(body.get("context", "")), str(body.get("questionId", "")))
            self._send(200, result)
        except (ValueError, json.JSONDecodeError, base64.binascii.Error) as exc:
            self._send(400, {"status": "error", "error": f"Invalid request: {exc}"})

    def _send(self, status: int, payload: dict) -> None:
        data = json.dumps(payload, ensure_ascii=False).encode()
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(data)))
        self.end_headers()
        self.wfile.write(data)


if __name__ == "__main__":
    print(f"Local AI service: http://{HOST}:{PORT}")
    print("Agents: " + ", ".join(AGENTS))
    ThreadingHTTPServer((HOST, PORT), Handler).serve_forever()
