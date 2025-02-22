# Git Workflow with `master`, `develop`, and `feature` Branches

## Overview
This workflow is optimized for **maximum security, simplicity, and efficiency**. There are three main branches:

- **`master`** → Only for stable, tested releases (never push directly)
- **`develop`** → Serves as a buffer layer for new features (merging happens here, not in `master`)
- **Feature Branches** (`feature/<name>`) → Each peer works in their own branch, derived from `develop`

## 1️⃣ Starting a New Task
Every new feature or bug fix starts in a dedicated branch:
```bash
git checkout develop
git pull origin develop
git checkout -b feature/<name>
```
**Visualization:**
```
A develop
     \
      B---C---D feature-branch
```

## 2️⃣ Working in the Feature Branch
- Work only in your branch
- Commit regularly
- Push your changes:
```bash
git push origin feature/<name>
```

## 3️⃣ Merging Changes into `develop`
Once the feature is completed, it is merged into `develop`:
```bash
git checkout develop
git pull origin develop
git merge feature/<name>
git push origin develop
```
**Visualization:**
```
develop
A--------------E (merge feature-branch)
     \                  /
      B---C---D feature-branch
```

If merge conflicts occur:
- Git will indicate the affected files
- Resolve the conflicts, save the files, and run `git commit`

## 4️⃣ Synchronizing `develop` with `master` (Only at Milestones)
```bash
git checkout master
git pull origin master
git merge develop
git push origin master
```
**Visualization:**
```
      M1---M2---M3 master
           \
      A---E---F---G develop
```

## 5️⃣ Keeping Feature Branches Up to Date (Using `merge` Instead of `rebase`)
Fetch the latest changes from `develop` to avoid merge conflicts:
```bash
git checkout feature/<name>
git fetch origin develop
git merge origin/develop
```
If conflicts occur:
- Resolve the affected files
- Run `git commit`

**Visualization:**
```
A develop
     \
      B---C---D feature-branch

develop after Merge:
A--------------E (merge develop)
     \                  /
      B---C---D feature-branch
```

## Advantages of This Workflow
✅ **Security** → No direct changes in `master`, all modifications go through `develop`.
✅ **Clean History** → `merge` ensures a structured development process.
✅ **Efficiency** → Clear separation between development and stable releases.

🚀 **Ready for productive work!**
