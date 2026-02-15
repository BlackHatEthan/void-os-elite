# GitHub Authentication Guide

## The Keychain Password Prompt

When you see: **"git-credential-osxkeychain wants to access key 'github.com' in your keychain"**

### What Password to Enter

**Enter your macOS user account password** (the same password you use to log into your Mac).

This is NOT your GitHub password - it's your Mac's login password. macOS uses the keychain to securely store your GitHub credentials, and it needs your Mac password to unlock it.

### Options

## Option 1: Enter Your Mac Password (Easiest)

1. Enter your **Mac login password** in the prompt
2. Click "Always Allow" if you want to avoid this prompt in the future
3. Your GitHub credentials will be saved securely in the keychain

## Option 2: Use Personal Access Token (Recommended)

If you don't want to use the keychain, use a Personal Access Token instead:

### Step 1: Create Personal Access Token

1. Go to GitHub: https://github.com/settings/tokens
2. Click **"Generate new token"** → **"Generate new token (classic)"**
3. Give it a name: "Void OS Elite"
4. Select scopes: Check **"repo"** (this gives full repository access)
5. Click **"Generate token"**
6. **COPY THE TOKEN IMMEDIATELY** (you won't see it again!)

### Step 2: Use Token Instead of Password

When you run `git push`, it will ask for:
- **Username**: `GreyHackx` (your GitHub username)
- **Password**: Paste your **Personal Access Token** (not your GitHub password)

```bash
git push -u origin main
# Username: GreyHackx
# Password: [paste your token here]
```

### Step 3: Save Token (Optional)

You can save the token so you don't have to enter it every time:

```bash
# Store credentials (will ask for username and token)
git config --global credential.helper osxkeychain

# Or use cache (stores for 15 minutes)
git config --global credential.helper cache
```

## Option 3: Use SSH Instead (No Password Prompts)

SSH keys don't require passwords once set up:

### Step 1: Generate SSH Key (if you don't have one)

```bash
ssh-keygen -t ed25519 -C "your_email@example.com"
# Press Enter to accept default location
# Press Enter twice for no passphrase (or set one)
```

### Step 2: Add SSH Key to GitHub

```bash
# Copy your public key
cat ~/.ssh/id_ed25519.pub
# Copy the entire output
```

1. Go to GitHub: https://github.com/settings/keys
2. Click **"New SSH key"**
3. Title: "Mac - Void OS"
4. Paste your public key
5. Click **"Add SSH key"**

### Step 3: Change Remote to SSH

```bash
# Remove HTTPS remote
git remote remove origin

# Add SSH remote
git remote add origin git@github.com:GreyHackx/void-os-elite.git

# Push (no password needed!)
git push -u origin main
```

## Option 4: Disable Keychain (Not Recommended)

If you really don't want to use keychain:

```bash
# Remove keychain helper
git config --global --unset credential.helper

# You'll need to enter credentials every time
```

## Quick Fix: Just Enter Your Mac Password

**Simplest solution**: Just enter your **Mac login password** when prompted. This is the password you use to unlock your Mac when you start it up.

Click **"Always Allow"** to avoid future prompts.

---

## Summary

- **Keychain password** = Your **Mac login password**
- **GitHub password** = Your GitHub account password (or use Personal Access Token)
- **Best option**: Enter Mac password, or use SSH keys for no prompts

---

**Recommended**: Just enter your Mac password and click "Always Allow" - it's the easiest!

