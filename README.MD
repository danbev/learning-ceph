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
This the what actually stores the data on a file system and providing access to
this data. The data that these OSDs store has an identifier, binary data, and
metadata consiting of key/value pairs.
This data can be accessed through the Ceph API, using Amazon S2 services,
or using the provided REST based API.


### Librados
Is a library allowing direct access to RADOS.

### Radosgw
Is a REST-based gateway.

### Controlled Replication Under Scalable Hashing (CRUSH)
This is the name of the algorighm that Ceph uses which is what determines where
to place data. The CRUSH map provides a view of what the cluster looks like.


### Clients
Before a client can start storing/retreiving data it needs to contact one of
the monitors to get the various cluster maps. With the cluster map the client is
made aware of all the monitors in the cluster, all the OSDs, and the metadata
daemons in the cluster. But it does not know anything about where objects are
stored. The location of where data is stored is calculated/computed.

Once it has these it can operate independently as the client is cluster aware.
The client writes an object to a pool and specifies an object id.



TODO: continue fleshing out components that make up Ceph

### Installing Ceph
This section will go through installing Ceph on Fedora. Most examples that I've
come accross use a tool named `ceph-deploy` but this tool is no longer maintained
and does not work with my version of Fedora ([link](https://ceph.io/ceph-management/introducing-cephadm/)).


Install `cephadm`:
```console
$ curl --silent --remote-name --location https://github.com/ceph/ceph/raw/octopus/src/cephadm/cephadm
```

Install Ceph tools:
```console
$ sudo dnf install fmt
```
Create `/etc/yum.repos.d/ceph.repo` with the following content:
```
[Ceph]
name=Ceph $basearch
baseurl=https://download.ceph.com/rpm-octopus/el8/$basearch
enabled=1
gpgcheck=1
gpgkey=https://download.ceph.com/keys/release.asc

[Ceph-noarch]
name=Ceph noarch
baseurl=https://download.ceph.com/rpm-octopus/el8/noarch
enabled=1
gpgcheck=1
gpgkey=https://download.ceph.com/keys/release.asc

[Ceph-source]
name=Ceph SRPMS
baseurl=https://download.ceph.com/rpm-octopus/el9/SRPMS
enabled=1
gpgcheck=1
gpgkey=https://download.ceph.com/keys/release.asc
```
There is an option to have this repo file created named `add-repo` but there
was no such build for fedora, so I'm currently using `el8` and hoping for the 
best.

```
$ sudo ./cephadm install ceph-common
INFO:cephadm:Installing packages ['ceph-common']...
$ ceph -v
```

Starting a Ceph cluster:
```console
$ sudo ./cephadm bootstrap --mon-ip 192.168.1.74 --allow-fqdn-hostname
```

Get the status of the Ceph cluster:
```console
$ sudo ceph -s
  cluster:
    id:     62418068-9a65-11ea-b0db-482ae34923e7
    health: HEALTH_WARN
            2 stray daemons(s) not managed by cephadm
            1 stray host(s) with 2 daemon(s) not managed by cephadm
            Reduced data availability: 1 pg inactive
            OSD count 0 < osd_pool_default_size 3
 
  services:
    mon: 1 daemons, quorum localhost.localdomain (age 18m)
    mgr: localhost.localdomain.tdctad(active, since 18m)
    osd: 0 osds: 0 up, 0 in
 
  data:
    pools:   1 pools, 1 pgs
    objects: 0 objects, 0 B
    usage:   0 B used, 0 B / 0 B avail
    pgs:     100.000% pgs unknown
             1 unknown
```
Note that is you forget the `sudo` command you'll get the following error:
```console
$ ceph -s
[errno 13] error connecting to the cluster
```


Accessing the Ceph cluster:
```console
$ sudo ./cephadm shell
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
