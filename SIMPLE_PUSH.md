# SIMPLE FIX - Follow These Steps Exactly

## The Problem
GitHub doesn't accept passwords anymore. You need a **Personal Access Token**.

## Solution (3 Steps)

### Step 1: Create Personal Access Token

1. **Open this link**: https://github.com/settings/tokens
2. Click **"Generate new token"** → **"Generate new token (classic)"**
3. **Name**: `Void OS Elite`
4. **Expiration**: Choose how long (90 days, 1 year, etc.)
5. **Select scopes**: Check **"repo"** (this gives full repository access)
6. Scroll down and click **"Generate token"**
7. **COPY THE TOKEN IMMEDIATELY** - it looks like: `ghp_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx`
   - You won't see it again!
   - Save it somewhere safe

### Step 2: Run These Commands

```bash
cd "/Users/greycipher/OSINT TOOL"

# Make sure remote is correct
git remote set-url origin https://github.com/BlackHatEthan/void-os-elite.git

# Make sure branch is main
git branch -M main

# Push
git push -u origin main
```

### Step 3: Enter Credentials

When it asks:
- **Username**: `BlackHatEthan`
- **Password**: **PASTE YOUR TOKEN HERE** (the `ghp_xxxxx` token you copied)

**Important**: Use the TOKEN as the password, NOT your GitHub password!

---

## That's It!

After entering the token, your code will push successfully.

---

## If You Still Have Issues

### Option: Use Token in URL (One-time)

```bash
# Replace YOUR_TOKEN with your actual token
git remote set-url origin https://YOUR_TOKEN@github.com/BlackHatEthan/void-os-elite.git
git push -u origin main
```

This embeds the token in the URL (less secure, but works).

---

**TL;DR**: 
1. Get token from https://github.com/settings/tokens
2. Run `git push -u origin main`
3. Username: `BlackHatEthan`, Password: **paste your token**

