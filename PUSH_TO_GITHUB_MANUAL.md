# Manual GitHub Push Instructions

## Quick Steps to Push Void OS Elite to GitHub

### Step 1: Make Script Executable (if needed)

```bash
cd "/Users/greycipher/OSINT TOOL"
chmod +x PUSH_TO_GITHUB.sh
```

### Step 2: Create GitHub Repository First

1. Go to https://github.com/new
2. Repository name: `void-os-elite`
3. Description: "High-performance, developer-grade, security-aware CLI operating system"
4. Choose **Public** or **Private**
5. **IMPORTANT**: Do NOT check "Add a README file", "Add .gitignore", or "Choose a license" (we already have these)
6. Click **"Create repository"**

### Step 3: Run These Commands

Copy and paste these commands one by one:

```bash
# Navigate to project
cd "/Users/greycipher/OSINT TOOL"

# Initialize git (if not already done)
git init

# Add all files
git add .

# Create commit
git commit -m "Initial commit: Void OS Elite v1.0

- 1,000+ commands across 14 categories
- Network & WiFi management
- Kali tools integration
- Complete documentation
- Bootable ISO creation
- Created by Ethan Sharma - Wadeson"

# Set branch to main
git branch -M main

# Add GitHub remote (replace GreyHackx with your username if different)
git remote add origin https://github.com/GreyHackx/void-os-elite.git

# Push to GitHub
git push -u origin main
```

### Alternative: Use the Script

If the script works, you can use:

```bash
cd "/Users/greycipher/OSINT TOOL"
bash PUSH_TO_GITHUB.sh GreyHackx
```

Or make it executable first:

```bash
chmod +x PUSH_TO_GITHUB.sh
./PUSH_TO_GITHUB.sh GreyHackx
```

### If You Get Authentication Errors

If `git push` asks for credentials:

1. **Use Personal Access Token** (recommended):
   - Go to GitHub → Settings → Developer settings → Personal access tokens → Tokens (classic)
   - Generate new token with `repo` permissions
   - Use token as password when pushing

2. **Or use SSH**:
   ```bash
   # Change remote to SSH
   git remote set-url origin git@github.com:GreyHackx/void-os-elite.git
   git push -u origin main
   ```

### Verify It Worked

After pushing, visit:
https://github.com/GreyHackx/void-os-elite

You should see all your files there!

---

**Note**: Make sure you create the GitHub repository FIRST (Step 2) before running the git commands!

