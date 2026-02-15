#!/bin/bash
# Quick script to push Void OS Elite to GitHub
# Usage: ./PUSH_TO_GITHUB.sh YOUR_GITHUB_USERNAME

set -e

GITHUB_USER="${1}"
REPO_NAME="void-os-elite"

if [ -z "$GITHUB_USER" ]; then
    echo "Usage: $0 <your_github_username>"
    echo "Example: $0 johndoe"
    exit 1
fi

echo "=========================================="
echo "  Void OS Elite - GitHub Push Script"
echo "=========================================="
echo ""
echo "This will:"
echo "  1. Add all files to git"
echo "  2. Create initial commit"
echo "  3. Add GitHub remote"
echo "  4. Push to GitHub"
echo ""
echo "Repository: https://github.com/${GITHUB_USER}/${REPO_NAME}"
echo ""
read -p "Continue? (yes/no): " confirm

if [ "$confirm" != "yes" ]; then
    echo "Aborted."
    exit 1
fi

# Check if git is initialized
if [ ! -d .git ]; then
    echo "Initializing git repository..."
    git init
fi

# Add all files
echo "Adding files to git..."
git add .

# Check if there are changes
if git diff --staged --quiet; then
    echo "No changes to commit. Repository is up to date."
else
    echo "Creating initial commit..."
    git commit -m "Initial commit: Void OS Elite v1.0

- 1,000+ commands across 14 categories
- Network & WiFi management
- Kali tools integration
- Complete documentation and tutorials
- Bootable ISO creation support
- Developer-focused CLI operating system
- Comprehensive help system (void-help)
- Native C/Assembly implementation"
fi

# Set branch to main
git branch -M main 2>/dev/null || true

# Remove existing remote if any
git remote remove origin 2>/dev/null || true

# Add GitHub remote
echo "Adding GitHub remote..."
git remote add origin "https://github.com/${GITHUB_USER}/${REPO_NAME}.git"

echo ""
echo "=========================================="
echo "  Ready to Push!"
echo "=========================================="
echo ""
echo "IMPORTANT: First create the repository on GitHub:"
echo "  1. Go to https://github.com/new"
echo "  2. Repository name: ${REPO_NAME}"
echo "  3. Description: High-performance, developer-grade, security-aware CLI operating system"
echo "  4. DO NOT initialize with README, .gitignore, or license"
echo "  5. Click 'Create repository'"
echo ""
echo "Then run:"
echo "  git push -u origin main"
echo ""
echo "Or if you've already created the repository, push now:"
read -p "Push to GitHub now? (yes/no): " push_now

if [ "$push_now" = "yes" ]; then
    echo "Pushing to GitHub..."
    git push -u origin main
    echo ""
    echo "=========================================="
    echo "  Successfully pushed to GitHub!"
    echo "=========================================="
    echo ""
    echo "Repository: https://github.com/${GITHUB_USER}/${REPO_NAME}"
else
    echo ""
    echo "When ready, run:"
    echo "  git push -u origin main"
fi

