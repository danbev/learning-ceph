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

#### Bootstrapping a monitor
A monitor as a unique id this called `fsid` which stands for File System ID which
was a little strange until I read that this is a remainder from the days when
Ceph mostly a Ceph FS.

A monitor also needs to know the clustername (the default is ceph).
Monitor name can be set but defaults to the hostname of the node it runs on.

A monitor map is generated using the fsid, clustername, and one or more hostname
and IP addresses.

Monitor keyring for communication with other monitors. This must be provided
when bootstrapping the initial monitor.

Administrator keyring which is used by the ceph CLI tools, which requires a
user named client.admin. This user must also be added to the monitor keyring.

### Object Store Daemons
This the what actually stores the data on a file system and providing access to
this data. The data that these OSDs store has an identifier, binary data, and
metadata consiting of key/value pairs.
This data can be accessed through the Ceph API, using Amazon S2 services,
or using the provided REST based API.

### Pools
Are logical groups of ceph objects.


### Manager
TODO:

### Librados
Is a library allowing direct access to RADOS.

### Rados Object Gateway
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

### Cluster maps
Both ceph clients and OSD daemons depend on having knowledge about the cluster.
This is information is provided in five maps.

#### Monitor map
```console
$ ceph mon dump
dumped monmap epoch 1
epoch 1
fsid 82caea9a-9fd3-11ea-b761-482ae34923e7
last_changed 2020-05-27T04:35:41.649530+0000
created 2020-05-27T04:35:41.649530+0000
min_mon_release 15 (octopus)
0: [v2:192.168.1.74:3300/0,v1:192.168.1.74:6789/0] mon.localhost.localdomain
```

#### OSD map
```console
ceph osd dump
epoch 4
fsid 82caea9a-9fd3-11ea-b761-482ae34923e7
created 2020-05-27T04:35:43.049269+0000
modified 2020-05-27T04:35:58.723837+0000
flags sortbitwise,recovery_deletes,purged_snapdirs,pglog_hardlimit
crush_version 1
full_ratio 0.95
backfillfull_ratio 0.9
nearfull_ratio 0.85
require_min_compat_client jewel
min_compat_client jewel
require_osd_release octopus
pool 1 'device_health_metrics' replicated size 3 min_size 2 crush_rule 0 object_hash rjenkins pg_num 1 pgp_num 1 autoscale_mode on last_change 3 flags hashpspool,creating stripe_width 0 pg_num_min 1 application mgr_devicehealth
max_osd 0
blacklist 192.168.1.74:0/3903093624 expires 2020-05-28T04:35:58.723811+0000
blacklist 192.168.1.74:0/3640786923 expires 2020-05-28T04:35:58.723811+0000
blacklist 192.168.1.74:6801/1724551218 expires 2020-05-28T04:35:58.723811+0000
blacklist 192.168.1.74:0/4057683 expires 2020-05-28T04:35:58.723811+0000
blacklist 192.168.1.74:6800/1724551218 expires 2020-05-28T04:35:58.723811+0000
```

#### Placement Group (gp) map
```console
$ ceph pg dump
```

#### CRUSH Map
```console
$ ceph osd getcrushmap -o compressed_crush_map
1
$ crushtool -d compressed_crush_map -o decompressed-crushmap
```
Output of `decompressed-crushmap`:
```
# begin crush map
tunable choose_local_tries 0
tunable choose_local_fallback_tries 0
tunable choose_total_tries 50
tunable chooseleaf_descend_once 1
tunable chooseleaf_vary_r 1
tunable chooseleaf_stable 1
tunable straw_calc_version 1
tunable allowed_bucket_algs 54

# devices

# types
type 0 osd
type 1 host
type 2 chassis
type 3 rack
type 4 row
type 5 pdu
type 6 pod
type 7 room
type 8 datacenter
type 9 zone
type 10 region
type 11 root

# buckets
root default {
	id -1		# do not change unnecessarily
	# weight 0.000
	alg straw2
	hash 0	# rjenkins1
}

# rules
rule replicated_rule {
	id 0
	type replicated
	min_size 1
	max_size 10
	step take default
	step chooseleaf firstn 0 type host
	step emit
}

# end crush map
```


### Amazon Simple Storage Service (S3)
Is a cloud object protocol used with amazons online file storage web service.
Is not opensource.

### Swift
OpenSource and originally developed by Rackspace. 
A common usecase for Swift is to virtual machine images. 
Implemented in Python and perhaps one in Go (not clear on this yet).

### Installing Ceph
This section will go through installing Ceph on Fedora. Most examples that I've
come accross use a tool named `ceph-deploy` but this tool is no longer maintained
and does not work with my version of Fedora ([link](https://ceph.io/ceph-management/introducing-cephadm/)).

#### Install cephadm
```console
$ curl --silent --remote-name --location https://github.com/ceph/ceph/raw/octopus/src/cephadm/cephadm
$ chmod 744 cephadm
$ sudo ./cephadm add-repo --release octopus
```
Currently, there is now repository for `fc31` which I'm using, so `/etc/yum.repos.d/ceph.repo`
needs to be updated to use `el8` instead which I hope will work.

Next install `ceph`:
```console
$ sudo ./cephadm install
```
This will actually install an older version of ceph as there are package dependencies
that cannot be met. See commit history for notes about this (I'm trying to keep
unnecessary details out of this document).

I've needed to set my hostname to not be a fully qualified domain name before
beeing able to successfully bootstrap the cluster:
```
$ sudo hostname ip-192-168-1-74
$ sudo ./cephadm bootstrap --mon-ip 192.168.1.74
INFO:cephadm:Ceph Dashboard is now available at:

	     URL: https://ip-192-168-1-74:8443/
	    User: admin
	Password: 04t7wb1wkx

INFO:cephadm:You can access the Ceph CLI with:

	sudo ./cephadm shell --fsid f8fb78fe-a3f4-11ea-96f7-482ae34923e7 -c /etc/ceph/ceph.conf -k /etc/ceph/ceph.client.admin.keyring

INFO:cephadm:Please consider enabling telemetry to help improve Ceph:

	ceph telemetry on

For more information see:

	https://docs.ceph.com/docs/master/mgr/telemetry/

INFO:cephadm:Bootstrap complete.
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
This will also happen with a node client if you try to connect. For now, I've
just chmod:ed all the files in `/etc/ceph`:
```console
$ sudo chmod 777 /etc/ceph/*
```

For our experimention we need to create a pool:
```console
$ ceph osd pool create data
pool 'data' created
$ ceph osd lspools
```
This is the name we use in [index.js](./n-api/index.js).

#### io_ctx.write_full hangs
I'm able to connect to the cluster and create an IoCtx using the pool created
above, but when I try call [io_ctx.write_full](./n-api/src/ceph-napi.cc#L63) nothing
happens, the process hangs for ever and I've not been able to find anything in
the logs which could point me to the issue.
I should not the I have absolutely know idea if this should work, especially 
with regards to the configuration of the cluster and in particular the OSD created
above.

### Compiling Ceph
First clone ceph:
```console
$ git clone git@github.com:ceph/ceph
$ cd ceph
$ git submodule update --init --recursive
```
Next, we use cmake to generate make files:
```console
$ mkdir build
$ cd build
$ cmake ..
$ make -j8
```

### Start a Ceph cluster with vstart:
After having built ceph run the following command from the checked out ceph
repository:
```console
$ cd build
$ env MON=1 OSD=1 MDS=1 MGR=1 RGW=1 ../src/vstart.sh -n -d --localhost
...

setting up user testid
setting up s3-test users
setting up user tester

S3 User Info:
  access key:  0555b35654ad1656d804
  secret key:  h7GhxuBLTrlhVUyxSPUKUV8r/2EI4ngqJxD7iBdBYLhwluN30JaT3Q==

Swift User Info:
  account   : test
  user      : tester
  password  : testing

start rgw on http://localhost:8000
/home/danielbevenius/work/ceph/ceph/build/bin/radosgw -c /home/danielbevenius/work/ceph/ceph/build/ceph.conf --log-file=/home/danielbevenius/work/ceph/ceph/build/out/radosgw.8000.log --admin-socket=/home/danielbevenius/work/ceph/ceph/build/out/radosgw.8000.asok --pid-file=/home/danielbevenius/work/ceph/ceph/build/out/radosgw.8000.pid --debug-rgw=20 --debug-ms=1 -n client.rgw.8000 '--rgw_frontends=beast port=8000' 
vstart cluster complete. Use stop.sh to stop. See out/* (e.g. 'tail -f out/????') for debug output.

dashboard urls: https://127.0.0.1:41818
  w/ user/pass: admin / admin
restful urls: https://127.0.0.1:42818
  w/ user/pass: admin / b8937d0e-08f4-4ba0-a9f3-05ac3a4e93fe

```
Notice that you can copy the `access key` and `secret key` which can be use
to connect to the S3 interface/API. We also see the url and port the rados
gateway listens to (https://127.0.0.1:41818).

After this the status of the cluster can be checked using:
```console
$ ./bin/ceph -s
 cluster:
    id:     bf1a5449-b6c8-4af8-9ee7-9edf7f2a8782
    health: HEALTH_WARN
            7 pool(s) have no replicas configured
 
  services:
    mon: 1 daemons, quorum a (age 72s)
    mgr: x(active, since 67s)
    mds: a:1 {0=a=up:active}
    osd: 1 osds: 1 up (since 42s), 1 in (since 42s)
    rgw: 1 daemon active (8000)
 
  task status:
    scrub status:
        mds.a: idle
 
  data:
    pools:   7 pools, 193 pgs
    objects: 223 objects, 9.5 KiB
    usage:   2.0 GiB used, 99 GiB / 101 GiB avail
    pgs:     193 active+clean
 
  progress:
    PG autoscaler decreasing pool 7 PGs from 32 to 8 (0s)
      [............................] 
```

### Stopping cluster started with vstart:
```console
$ ../src/stop.sh
```

#### Stop a cluster started with cephadm
```
$ sudo ./cephadm rm-cluster --fsid=62418068-9a65-11ea-b0db-482ae34923e7 --force
```
You can use `./cephadm ls` to get the fsid.


### Installing librados
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
continue...
ceph-napi init...
client.admin
Connected to Ceph cluster
Going to write to pool data
Is io_ctx valid: 1
```
*WIP* Currently trying to write to the pool just hangs which is what I'm currently
trying to figure out...

### Ceph REST client
This section should look at how a Ceph REST gateway is set up and create an\
example client that uses it.
TODO:

### Native client vs REST client
The motivation for having a native node module for Ceph is that, from what I
understand, a Ceph client is cluster aware and can communicate directly
with OSD to read and write data. This could perhaps be useful in situaions where
a user wants to implement a function as a service that responds to some Knative
event. The function in this use case would not be exposed to the outside and
could use the native Ceph node module instead of using a REST based client.

A REST based client would be useful in other cases where user already familiar
with the S3 API and Swift. 
TODO: investigate the S3 API and Swift

### Journaling file system
Is when transaction or operations are stored in a special allocation on disk
allowing a write to be rolled-back if something happens to the write operation
while it is in progress.
