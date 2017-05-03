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

Set the new remote:
```sh
 git remote add origin https://github.com/farabiahmed/MachineLearningFramework.git
```

Verify the new remote url
```sh
git remote -v
```

Push the changes in your local repo up to remote that you specified as the origin
```sh
git push origin master
```