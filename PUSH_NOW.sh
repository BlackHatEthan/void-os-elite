#!/bin/bash
# Quick push script for BlackHatEthan/void-os-elite

cd "/Users/greycipher/OSINT TOOL"

echo "=========================================="
echo "  Pushing to GitHub"
echo "=========================================="
echo ""
echo "Repository: https://github.com/BlackHatEthan/void-os-elite"
echo ""

# Ensure remote is correct
git remote set-url origin https://github.com/BlackHatEthan/void-os-elite.git

# Ensure branch is main
git branch -M main

# Push
echo "Pushing to GitHub..."
git push -u origin main

echo ""
echo "=========================================="
echo "  Done!"
echo "=========================================="
echo ""
echo "Your repository: https://github.com/BlackHatEthan/void-os-elite"
echo ""

