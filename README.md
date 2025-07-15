# Suksma-Grahaka

# Suksma-Grahaka v0.5

Suksma-Grahaka is a lightweight Windows surveillance utility written in C++. It is designed to capture and exfiltrate data such as screenshots of browser activity and user keystrokes. This tool is intended strictly for **educational and research purposes** in malware analysis and red teaming environments.

---

## Features

- **Continuous Screenshot Capture**
  - Captures active window screenshots when the user is browsing in **Chrome** or **Firefox**.
  - Images are saved locally with timestamp-based filenames.

- **Keystroke Logging**
  - Captures all user keystrokes using a global low-level keyboard hook.
  - Stores the data in a persistent file for analysis.

- **Data Exfiltration Module**
  - (Optional) Transfers captured logs and screenshots from the victim’s machine to an attacker-controlled host (via custom network or file-transfer logic).

---

## Technical Stack

- Language: C++
- Windows API: `SetWindowsHookEx`, `BitBlt`, `CreateProcess`, `WinInet`, `GDI+`
- Targets: Windows 10 and above (x64)

---

## Directory Structure

```plaintext
Suksma-Grahaka/
├── capture/
│   ├── keystrokes.txt
│   └── image_<timestamp>.png
├── src/
│   └── main.cpp
├── README.md
└── ...
```

## Disclaimer
This project is intended strictly for ethical hacking research, malware behavior analysis, and red team simulation. Misuse of this tool in violation of any laws or against unwitting targets is strictly discouraged.

## Future Enhancements
- Add HTTP/WebSocket exfiltration module
- Encrypt captured data before transfer
- Implement stealth and evasion techniques
- Add persistence via Windows service/registry

## Follow Me
- [GitHub](https://github.com/SatvikHatulkar)
- [Medium](https://medium.com/@satvikhatulkar)
- [LinkedIn](https://www.linkedin.com/in/satvik-hatulkar-a91042252)