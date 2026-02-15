#!/bin/bash
# Final fix - Use Personal Access Token

cd "/Users/greycipher/OSINT TOOL"

echo "=========================================="
echo "  GitHub Push - Token Setup"
echo "=========================================="
echo ""
echo "GitHub requires a Personal Access Token (not password)"
echo ""
echo "STEP 1: Create Token"
echo "  1. Go to: https://github.com/settings/tokens"
echo "  2. Click 'Generate new token' → 'Generate new token (classic)'"
echo "  3. Name: Void OS Elite"
echo "  4. Check 'repo' scope"
echo "  5. Click 'Generate token'"
echo "  6. COPY THE TOKEN (you won't see it again!)"
echo ""
echo "STEP 2: After you have the token, press Enter to continue..."
read

# Set remote to HTTPS
git remote set-url origin https://github.com/BlackHatEthan/void-os-elite.git

# Ensure branch is main
git branch -M main

echo ""
echo "Now pushing... When prompted:"
echo "  Username: BlackHatEthan"
echo "  Password: [PASTE YOUR TOKEN HERE]"
echo ""
read -p "Press Enter to push..."

git push -u origin main

