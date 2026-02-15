# Fix GitHub Authentication Error

## The Problem

```
remote: Permission to BlackHatEthan/void-os-elite.git denied to vibecodingpilot-max.
```

Git is using cached credentials from a different GitHub account (`vibecodingpilot-max`) instead of `BlackHatEthan`.

## Solution: Clear Credentials and Re-authenticate

### Option 1: Clear Keychain Credentials (Recommended)

```bash
cd "/Users/greycipher/OSINT TOOL"

# Clear GitHub credentials from keychain
git credential-osxkeychain erase <<EOF
host=github.com
protocol=https
EOF

# Now push (will ask for new credentials)
git push -u origin main
```

When prompted:
- **Username**: `BlackHatEthan`
- **Password**: Your GitHub password or Personal Access Token

### Option 2: Use Personal Access Token

If you have 2FA enabled or want to use a token:

1. **Create Token:**
   - Go to: https://github.com/settings/tokens
   - Click "Generate new token (classic)"
   - Name: `Void OS Elite`
   - Check **"repo"** scope
   - Generate and **copy the token**

2. **Push with Token:**
   ```bash
   git push -u origin main
   # Username: BlackHatEthan
   # Password: [paste your token here]
   ```

### Option 3: Use SSH Instead (No Password Prompts)

1. **Generate SSH Key** (if you don't have one):
   ```bash
   ssh-keygen -t ed25519 -C "your_email@example.com"
   # Press Enter to accept defaults
   ```

2. **Add SSH Key to GitHub:**
   ```bash
   # Copy your public key
   cat ~/.ssh/id_ed25519.pub
   ```
   - Go to: https://github.com/settings/keys
   - Click "New SSH key"
   - Paste your public key
   - Save

3. **Change Remote to SSH:**
   ```bash
   git remote set-url origin git@github.com:BlackHatEthan/void-os-elite.git
   git push -u origin main
   ```

### Option 4: Manual Keychain Edit

1. Open **Keychain Access** app on Mac
2. Search for `github.com`
3. Delete any entries for `github.com`
4. Try pushing again

## Quick Fix Commands

Run these commands:

```bash
cd "/Users/greycipher/OSINT TOOL"

# Clear credentials
git credential-osxkeychain erase <<EOF
host=github.com
protocol=https
EOF

# Verify remote is correct
git remote set-url origin https://github.com/BlackHatEthan/void-os-elite.git

# Push
git push -u origin main
```

When it asks for credentials:
- Username: `BlackHatEthan`
- Password: Your GitHub password or token

---

**Recommended**: Use Option 1 (clear keychain) or Option 3 (SSH) for easiest setup.

