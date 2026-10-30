#!/usr/bin/env python3
"""One-shot opt-in Gemini image smoke test; never prints the API key."""
import base64
import json
import os
import socket
import sys
import urllib.error
import urllib.request
from pathlib import Path


def load_env(path: Path) -> None:
    if not path.exists():
        return
    for line in path.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if line and not line.startswith("#") and "=" in line:
            key, value = line.split("=", 1)
            os.environ.setdefault(key.strip(), value.strip().strip('"\''))


def urlopen_ipv4(request, timeout):
    """Use IPv4 first/only when the host has no usable IPv6 route."""
    original = socket.getaddrinfo

    def ipv4_only(host, port, family=0, type=0, proto=0, flags=0):
        return original(host, port, socket.AF_INET, type, proto, flags)

    socket.getaddrinfo = ipv4_only
    try:
        return urllib.request.urlopen(request, timeout=timeout)
    finally:
        socket.getaddrinfo = original


def main() -> int:
    root = Path(__file__).resolve().parents[1]
    load_env(root / ".env")
    api_key = os.environ.get("GEMINI_API_KEY", "").strip()
    model = os.environ.get("GEMINI_MODEL", "gemini-3.8-flash").strip()
    timeout_seconds = int(os.environ.get("GEMINI_TIMEOUT_SECONDS", "60"))
    image_path = root / "question.png"
    if not api_key:
        print("FAIL: GEMINI_API_KEY is not configured")
        return 2
    if not image_path.exists():
        print(f"FAIL: image not found: {image_path}")
        return 2

    # The PNG is sent as base64 inline image data; no separate OCR stage.
    encoded = base64.b64encode(image_path.read_bytes()).decode("ascii")
    prompt = (
        "Analyze this authorized practice question. Identify all visible choices and "
        "select the best-supported answer. Return JSON only with keys questionId, "
        "selectedLabel, status, confidence. status must be success, ambiguous, "
        "unreadable, unsupported, or error. Do not provide reasoning."
    )
    payload = {
        "contents": [{"parts": [
            {"text": prompt},
            {"inline_data": {"mime_type": "image/png", "data": encoded}},
        ]}],
        "generationConfig": {"responseMimeType": "application/json"},
    }
    url = f"https://generativelanguage.googleapis.com/v1beta/models/{model}:generateContent?key={api_key}"
    request = urllib.request.Request(
        url,
        data=json.dumps(payload).encode("utf-8"),
        headers={"Content-Type": "application/json"},
        method="POST",
    )
    try:
        with urlopen_ipv4(request, timeout_seconds) as response:
            body = json.loads(response.read().decode("utf-8"))
    except urllib.error.HTTPError as exc:
        print(f"FAIL: Gemini HTTP {exc.code}: {exc.read().decode('utf-8', errors='replace')[:500]}")
        return 1
    except urllib.error.URLError as exc:
        reason = exc.reason
        if isinstance(reason, socket.gaierror):
            print("FAIL: DNS cannot resolve generativelanguage.googleapis.com")
            print("FIX: check internet/DNS, proxy, firewall, or run this test on your local Windows machine")
        else:
            print(f"FAIL: network error: {reason}")
        return 1
    except TimeoutError:
        print(f"FAIL: Gemini request timed out after {timeout_seconds} seconds")
        print("FIX: check proxy/firewall/TLS access to generativelanguage.googleapis.com:443")
        return 1

    candidates = body.get("candidates", [])
    if not candidates:
        print("FAIL: Gemini returned no candidates")
        return 1
    parts = candidates[0].get("content", {}).get("parts", [])
    text = next((part.get("text", "") for part in parts if part.get("text")), "")
    try:
        result = json.loads(text)
    except json.JSONDecodeError:
        print(f"FAIL: model response was not JSON: {text[:500]}")
        return 1
    allowed = {"success", "ambiguous", "unreadable", "unsupported", "error"}
    status = result.get("status")
    if status not in allowed:
        print(f"FAIL: invalid status: {status!r}")
        return 1
    if status == "success" and not result.get("selectedLabel"):
        print("FAIL: success response has no selectedLabel")
        return 1
    print(json.dumps(result, ensure_ascii=False, indent=2))
    print("PASS: question.png was sent as one base64 inline-image request")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
