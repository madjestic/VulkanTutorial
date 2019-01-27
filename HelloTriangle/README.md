# build instructions
```bash
  $ cd ${project_dir}
  $ make 
```
# set up Tags:
```bash
  $ cd ${project_dir} && gtags .
  $ cd ${project_dir}/build
  $ rdm &                     # start a rtags daemon
  $ rc -J .                   # index the project with rtags
```
