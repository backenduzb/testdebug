# Local AI service (Arch + Hyprland)

This service needs no Gemini/DeepSeek API key. It uses the local authenticated
`codex` and `opencode` CLIs and sends the screenshot as a temporary file. It
binds only to `127.0.0.1`, serializes requests, retries transient failures, and
switches agents when a limit or process failure is detected.

The Qt application talks to this local service, while Codex/OpenCode themselves
may use their own internet connection. On Hyprland install `grim` for global
screen capture:

```bash
sudo pacman -S grim
```

## Start

Make sure both CLIs work interactively first, then run:

```bash
python3 local_service/server.py
```

Check it:

```bash
curl -s http://127.0.0.1:8765/health | jq
```

Send `question.png`:

```bash
base64 -w0 question.png > /tmp/question.png.b64
jq -n --rawfile image /tmp/question.png.b64 \
  '{imageBase64:$image,questionId:"demo-1",context:"authorized practice"}' \
  | curl -sS http://127.0.0.1:8765/v1/analyze \
      -H 'Content-Type: application/json' -d @- | jq
rm -f /tmp/question.png.b64
```

The image is deleted after the request. Agent output and API credentials are not
written to logs. `LOCAL_AI_TIMEOUT` controls the per-agent timeout (default 120s).

## Private remote access with Tailscale

For a private Tailscale network, access can be controlled by Tailscale ACLs and
the optional service token can be left unset. The desktop app then needs no
manual token entry and connects immediately after installation. If you use a
public Funnel URL, enable a token and a separate authenticated client.

Publish the localhost service only inside your Tailscale network:

```bash
tailscale serve --bg --https=443 http://127.0.0.1:8765
tailscale serve status
tailscale status
```

Install/login to Tailscale on the remote Windows computer too. Set the app's
Local service URL to the HTTPS hostname shown by `tailscale serve status`, ending
in `/v1/analyze`. No token entry is needed when Tailscale ACLs restrict the
tailnet. This is private tailnet access; every client must be authorized in the
same Tailscale network.

Only use `tailscale funnel` if you intentionally need a public internet URL; it
requires an authentication layer and should not be used for an unauthenticated
agent service.
