#!/bin/bash
# Quick GitHub push - run with: bash QUICK_PUSH.sh

cd "/Users/greycipher/OSINT TOOL"

echo "Step 1: Adding files..."
git add .

echo "Step 2: Creating commit..."
git commit -m "Initial commit: Void OS Elite v1.0 - Created by Ethan Sharma - Wadeson" || echo "Already committed or no changes"

echo "Step 3: Setting branch to main..."
git branch -M main 2>/dev/null || true

echo ""
echo "=========================================="
echo "  Next Steps:"
echo "=========================================="
echo ""
echo "1. Create repository on GitHub:"
echo "   https://github.com/new"
echo "   Name: void-os-elite"
echo "   DO NOT initialize with README/license"
echo ""
echo "2. Then run these commands:"
echo ""
echo "   git remote add origin https://github.com/GreyHackx/void-os-elite.git"
echo "   git push -u origin main"
echo ""
echo "Or if remote already exists:"
echo "   git push -u origin main"
echo ""

