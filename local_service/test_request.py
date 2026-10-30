#!/usr/bin/env python3
"""Memory-safe local smoke test; avoids putting base64 in a shell argument."""
import base64
import json
import sys
import urllib.request
from pathlib import Path

image = Path(sys.argv[1] if len(sys.argv) > 1 else "question.png")
payload = json.dumps({
    "imageBase64": base64.b64encode(image.read_bytes()).decode("ascii"),
    "questionId": "demo-1",
    "context": "authorized practice",
}).encode()
request = urllib.request.Request(
    "http://127.0.0.1:8765/v1/analyze",
    data=payload,
    headers={"Content-Type": "application/json"},
    method="POST",
)
with urllib.request.urlopen(request, timeout=180) as response:
    print(response.read().decode())
