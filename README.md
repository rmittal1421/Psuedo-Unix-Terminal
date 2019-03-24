# CMPT300- Password for each pdf page - caprica


# Pseudo-Unix-Terminal

This project emulates the unix shell command 'ls'. It implements the following variations of the command:
- -i
- -l
- -R

These work in exactly the same way as it would in a unix terminal. For instance:
- ls -l: List in long format (see below). A total sum of all file sizes is output on a line before the long listing. Output is one entry per line.
- ls -i: For each file, print its inode number.
- ls -R: Recursively list subdirectories encountered.

# How to Start the Application

Please follow the steps below to run the application:
- Clone the repository using: git clone `https://github.com/rmittal1421/Psuedo-Unix-Terminal.git`
- Compile the application using: `make` (MakeFile is included in the application)
- Run the application using './a.out' and this would trigger the application.
- Now, you can use the pseudo-terminal (application runtime) as a normal terminal to run the described variations of `ls` command.

# Contributing to the Application

There are several scopes of scalability in this application. We can incorporate other linux commands to become part of the our pseudo-Unix-Terminal. If you see any bugs in the application or have any exciting ideas, feel free to contribute to the project by creating a pull request as described below:
- Clone the repository using `https://github.com/rmittal1421/Psuedo-Unix-Terminal.git`
- Make changes in a feature branch
- Create a pull request for your changes

If approved, your changes would become part of this application.
