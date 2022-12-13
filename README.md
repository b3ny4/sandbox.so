
# LD\_PRELOAD sandbox
-------------------

The Sandbox overwrites common libc functions and stumps to prevent access on critical files. As a test, my own implementation of cat tries to access the flag.txt. 

Testes on:
 - Ubuntu 22.04 (Jammy Jellyfish)

## configure the Sandbox
The sandbox first looks for the environment variable `SANDBOX_CONFIG`. If specified, it looks for the files `whitelist` and `blacklist` in there.
If the variable is not set, it looks in the `CWD` for the two files.

The lists need the following syntax:

 - Every line specifies one file
 - Each file has to be an absolute path (directories without a final slash)
 - Lines are separated by `\n`
 - The list ends with a newline character (even if empty!)

If a file does not appear on either list, the sandbox asks the user what to do about it.

To activate the sandbox, preload it

```
$ export LD_PRELOAD=$(pwd)/sandbox.so
```

## Hacking the Sandbox

### dlsym and other calls
Since out sandbox is just a library, not a framework, we can do what the sandbox can do and just load our own version of the libc.
cat_exploit1 implements a POC. You can mittigate this by overwriting dlsym as well, but at some point you have to overwrite almost the entire libc and dlsym is a tough one to overwrite since the library itself then could not access libc anymore.

You might as well find a libc call I didn't overwrite due to the large ammount of possibilities. The LD_PRELOAD sandbox ist designed to only hook the most common ones. Libc is evolving and a sandbox must be compliant with the local libc if you want to prevent this.

### Syscall
libc is just a library and instead of finding and loading the original one, we can implement it ourselves. Okay. Maybe not the entire one, but the stuff we need. cat_exploit_2 circumvents the sandbox by calling the syscall directly in assembler (x86_64 only!)

### cut off sandbox
Another way is to simply delete LD_PRELOAD and restart the process. cat_exploit3 implements a working prove of concept for that.

Herem the same limitations as for dlsym and other calls apply. If the sandbox checks them as well, the exploit won't work.

### static linking
libc can be linked statically. This way the program never loads the sandbox and the sandbox has no chance intercepting the calls. cat_exploit4 implements this attack.

cat_exploit4 has no source code since it reuses cat.c. To see the attack, please refer to the Makefile (read at your own risk)
