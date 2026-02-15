#!/bin/bash
# Fix GitHub authentication for BlackHatEthan

cd "/Users/greycipher/OSINT TOOL"

echo "=========================================="
echo "  Fixing GitHub Authentication"
echo "=========================================="
echo ""

# Remove cached credentials
echo "Step 1: Clearing cached credentials..."
git credential-osxkeychain erase <<EOF
host=github.com
protocol=https
EOF

# Update remote URL
echo "Step 2: Setting correct remote..."
git remote set-url origin https://github.com/BlackHatEthan/void-os-elite.git

# Ensure branch is main
echo "Step 3: Setting branch to main..."
git branch -M main

echo ""
echo "=========================================="
echo "  Ready to Push"
echo "=========================================="
echo ""
echo "Now run:"
echo "  git push -u origin main"
echo ""
echo "When prompted:"
echo "  Username: BlackHatEthan"
echo "  Password: Your GitHub password or Personal Access Token"
echo ""

