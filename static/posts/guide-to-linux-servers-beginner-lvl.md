# A Guide to setting up a fresh Linux server for Beginners:

Whether you are looking for some independance or you are simply trying to experiment with Linux Servers, and you don't have a lot of experience setting up Servers, I will try to condense as much as possible all the necessary steps for setting up a new Server. 

** This exlcudes the buying/renting part** 

## Step 1: `ssh` into your instance.

the first thing we need to do is get your IP address. Wherever you rented your server, you'll get an Address, in the form of an IP that corresponds to your server. You also will have set up initially a password for the root user. So let's get those details

In a new terminal run: 

```bash 
ssh root@123.45.123.345
```

Type your password and if all goes well you'll get a nice message displaying Welcome to your server.

## Step 2: Updating your packages.

Ideally, you know what your distro version is, but if not, go ahead and type this

```bash
cat /etc/os-release
```
![Cat Version](/img/cat-version-ubuntu.png)
