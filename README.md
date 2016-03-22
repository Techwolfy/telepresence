Telepresence [![Build Status](https://travis-ci.org/Techwolfy/telepresence.svg?branch=master)](https://travis-ci.org/Techwolfy/telepresence)
============


What is Telepresence?
---------------------

> tel·e·pres·ence /ˈteləˌprezəns/ noun: the use of virtual reality technology, especially for remote control of machinery or for apparent participation in distant events.

Telepresence is a project designed to provide an easy method of communicating with a robot remotely. It can be used either with a direct connection between client and robot, or with both client and robot connecting to a central server (e.g. to bypass firewalls when connecting over the internet).


Building Telepresence
---------------------

Currently, only Linux operating systems are supported. Telepresence itself depends only on [`JsonCpp`](https://github.com/open-source-parsers/jsoncpp) (for packet serialization and deserialization), but its input and output modules depend on the various libraries associated with their hardware.

To build telepresenced and the various client and robot binaries, simply run `make`.


Using Telepresence
------------------

While exact usage instructions depend on the individual modules and hardware, the basics remain the same. If using the client/server/robot model, ensure the daemon is running and accessible over the network, then start the client and robot with its network address to begin communication. If using the client/robot model, either run each with the network address of the other or set one to autodetect and point the other to it. Once the system is running, the robot should be remotely controllable from the client.

The server is only intended to echo packets from the client to the robot when both are behind restrictive firewalls, or when neither will have a static IP address or hostname. If using the web client, the client should be run from an accessible network location in listening mode instead of the server.


Alternate Clients
-----------------

Telepresence network packets use a simple JSON protocol, so it is relatively simple to create alternate client and robot software. The web client included in this repository uses a daemon component due to cross-domain restrictions; that interface is also available, but is slightly less useful than the full protocol. A fully independent Android implementation of the client is available in the [`TelepresenceRemote`](https://github.com/Techwolfy/TelepresenceRemote) repository.


License
-------

Telepresence is released under the GNU General Public License, version 3.0. For more informaion, please see [`COPYING`](COPYING).


TODO
----

Pull requests welcome! Please try to keep the same code and commit message style.

* Add more modules for different types of input and output
* Test non-dummy output modules
* Add some method of bidirectional streaming for video and audio (e.g. via Youtube or Twitch)
* Support Windows builds with MinGW
