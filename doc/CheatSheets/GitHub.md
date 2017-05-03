# Git Hub Cheat Sheet

Get what is going on in git

```sh
git status
```

Stage the files
```
git add -v .
```
* -v is to see which files staged.

Unstage a directory that is staged mistakenly:

```sh
git rm --cached -r <dir>
```
* --cached switch makes ```git rm``` operate on the index only
* -r is for “recursive”.

Commit the files that you staged in your local repo:
```sh
git commit -m "First commit"
```

