# Live Gemini smoke test

Run from the repository root:

```bash
python3 tests/gemini_live_test.py
```

The script reads `.env`, sends `question.png` once as base64 `image/png`, validates the structured response, and never prints the API key. This is a live API test and consumes one Gemini request; it is intentionally not part of CTest.
