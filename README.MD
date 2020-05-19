### Learning Ceph storage
This sole purpose of this project is to learn about ceph storage and figure out
the best way use Ceph from Node.js

### Background
This section will contains information about Ceph as I don't know anything at
all about it, nor am I very familiar with storage solutions either.

Ceph is a storage system that does not have a centralized metadata server and
uses a an algorithm called Controlled Replication Under Scalable Hashing (CRUSH).
Instead of asking a central metadata server where data is stored the location
of data is calculated.

There are 3 types of storage that Ceph provides:
* Block storage via RADOS Block Device (RBD)
* Object storage via RADOS Gateway
* File storage via CephFS

### Reliable Autonomous Distributed Object Store (RADOS)
This is the object store upon which everything else is built. This layer contains
the Object Store Daeomons (OSD). These daemons are totally independant and form
peer-to-peer relationships to form a cluster (sounds similar to what is/was
possible when using jgroups).
An OSD is normally mapped to a single disk (where traditional approaches would
use multiple disks and a RAID controller).

### Storage cluster
A storage cluster consists of monitors, and OSD daemons.

### Monitors
These are responsible for providing a known cluster state including membership
and use cluster maps for this. Cluster clients retrives the cluster map from one
of the monitors.
OSDs also report back there state to the monitors.

### Manager
TODO

### Object Store Daemons
The data that these store has an identifier, binary data, and metadata consiting
of key/value pairs.


### Librados
Is a library allowing direct access to RADOS

### Radosgw
Is a REST-based gateway.


### Clients
Before a client can start storing/retreiving data it needs to contact one of
the monitors to get the various cluster maps. Once it has these it can operate
independently as the client is cluster aware. So they can talk directly to the 
OSDs.


TODO: continue fleshing out components that make up Ceph

### Installing Ceph
This section will go through installing Ceph on Fedora

```console
$ curl --silent --remote-name --location https://github.com/ceph/ceph/raw/octopus/src/cephadm/cephadm
$ chmod -x cephadm
```

Installing librados:
```console
$ sudo dnf install -y librados-devel
```

### Ceph-napi
The [n-api](./n-api) directory contains an example of what a node native addon
written using N-API might look like.

This native module use [Librados](https://docs.ceph.com/docs/master/rados/api/librados/).


#### Building
```console
$ cd n-api
$ npm i
$ npm run compile
$ node index.js
librados version: 3.0.0
```
