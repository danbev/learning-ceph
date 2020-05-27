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

Each object is a stream of binary data which is associated key when created.

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


### Amazon Simple Storage Service (S3)
Is a cloud object protocol used with amazons online file storage web service.

### Swift
Like S3 is a cloud object protocol but from the OpenStack foundation.

TODO: continue fleshing out components that make up Ceph

### Installing Ceph
This section will go through installing Ceph on Fedora. Most examples that I've
come accross use a tool named `ceph-deploy` but this tool is no longer maintained
and does not work with my version of Fedora ([link](https://ceph.io/ceph-management/introducing-cephadm/)).

I'm using Fedora 31 and the latest cephadm is compatible (as far as I can tell)
with version 15.2.1 of Ceph. But trying to install 15.2.1 on Fedora does not
work (see below for details) and will fallback to installing 14.9.2 which is
easy to miss. I've not been able to work around this so I opted to compile
ceph locally instead (https://github.com/ceph/ceph#checking-out-the-source).
After building, which takes a while, and running `make install` I got the following
error when running ceph
```console
$ which ceph
/usr/local/bin/ceph
$ ceph --version
Traceback (most recent call last):
  File "/usr/local/bin/ceph", line 151, in <module>
    from ceph_daemon import admin_socket, DaemonWatcher, Termsize
  File "/usr/local/lib/python3.7/site-packages/ceph_daemon.py", line 24, in <module>
    from prettytable import PrettyTable, HEADER
ModuleNotFoundError: No module named 'prettytable'
```
This can be fixed bu installing `PrettyTable`:
```console
$ sudo pip install PrettyTable
```
```console
$ ceph --version
ceph version 16.0.0-1852-g79c7ac53e6 (79c7ac53e671282ba8019f663e1449663342bfbd) pacific (dev)
```

Remove ceph if it is already installed.
```console
$ sudo dnf remove ceph-common
```

Install `cephadm`:
```console
$ curl --silent --remote-name --location https://github.com/ceph/ceph/raw/octopus/src/cephadm/cephadm
$ chmod 744 cephadm
```

```console
$ sudo ./cephadm add-repo --version 15.2.1
INFO:root:Writing repo to /etc/yum.repos.d/ceph.repo...
```
There was no repository for Fedora 31 which I'm using so I updated the above
`ceph.repo` file to use `el8` instead.

Install ceph tools:
```console
$ sudo dnf install ceph-common
Problem: cannot install the best candidate for the job
  - nothing provides /usr/libexec/platform-python needed by ceph-common-2:15.2.1-0.el8.x86_64
```

Starting a Ceph cluster:
```console
$ sudo ./cephadm bootstrap --mon-ip 192.168.1.74 --allow-fqdn-hostname
```

Get ip:
```console
$ ip addr show
```

```console
$ sudo ./cephadm ls
[
    {
        "style": "cephadm:v1",
        "name": "mon.localhost.localdomain",
        "fsid": "62418068-9a65-11ea-b0db-482ae34923e7",
        "enabled": true,
        "state": "running",
        "container_id": "648a087052751e5a40a5048f5a04964b0d105f16d96571b0413795809eb2eeb8",
        "container_image_name": "docker.io/ceph/ceph:v15",
        "container_image_id": "4569944bb86c3f9b5286057a558a3f852156079f759c9734e54d4f64092be9fa",
        "version": "15.2.2",
        "started": "2020-05-20T06:45:20.837583",
        "created": "2020-05-20T06:45:17.648583",
        "deployed": "2020-05-20T06:45:16.743583",
        "configured": "2020-05-20T06:45:17.648583"
    },
    {
        "style": "cephadm:v1",
        "name": "mgr.localhost.localdomain.tdctad",
        "fsid": "62418068-9a65-11ea-b0db-482ae34923e7",
        "enabled": true,
        "state": "running",
        "container_id": "74fe45114309474f286e7060b590457104daa247654d14170e6c8eac456a4ea3",
        "container_image_name": "docker.io/ceph/ceph:v15",
        "container_image_id": "4569944bb86c3f9b5286057a558a3f852156079f759c9734e54d4f64092be9fa",
        "version": "15.2.2",
        "started": "2020-05-20T06:45:21.822435",
        "created": "2020-05-20T06:45:21.757585",
        "deployed": "2020-05-20T06:45:21.338585",
        "configured": "2020-05-20T06:45:21.757585"
    }
]
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

Adding and OSD:
```console
$
```

Accessing the Ceph cluster:
```console
$ sudo ./cephadm shell
```

To stop a cluster:
```
$sudo ./cephadm rm-cluster --fsid=62418068-9a65-11ea-b0db-482ae34923e7 --force
```
You can use `./cephadm ls` to get the fsid.


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

### Journaling file system
Is when transaction or operations are stored in a special allocation on disk
allowing a write to be rolled-back if something happens to the write operation
while it is in progress.
