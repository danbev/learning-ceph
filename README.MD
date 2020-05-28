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

#### Configure a monitor
The following steps go through and initial monitor on my laptop.

I tried using a string as the value for `fsid` but later failed. Instead I used
`uuidgen`(:r!uuidgen):
```
[global]
fsid = 9820a792-a2d8-42a0-a7e5-6e23a2bc9d69
mon initial members = localhost.localdomain
mon host = 192.168.1.74
public network = 192.168.1.1/24
auth cluster required = cephx
auth service required = cephx
auth client required = cephx
osd journal size = 1024
osd pool default size = 1
osd pool default min size = 1
osd pool default pg num = 333
osd pool default pgp num = 333
osd crush chooseleaf type = 1
```

Create the keyring for the monitor:
```console
$ sudo ceph-authtool --create-keyring /tmp/ceph.mon.keyring --gen-key -n mon. --cap mon 'allow *'
```

Create the keyring for the client cli tools:
```console
$ sudo ceph-authtool --create-keyring /etc/ceph/ceph.client.admin.keyring --gen-key -n client.admin --cap mon 'allow *' --cap osd 'allow *' --cap mds 'allow *' --cap mgr 'allow *'
```

Create the bootstrap-osd keyring:
```console
$ mkdir -p /var/lib/ceph/bootstrap-osd/
$ sudo ceph-authtool --create-keyring /var/lib/ceph/bootstrap-osd/ceph.keyring --gen-key -n client.bootstrap-osd --cap mon 'profile bootstrap-osd' --cap mgr 'allow r'
```

Add the client.admin.key to the monitor keyring:
```console
$ ceph-authtool /tmp/ceph.mon.keyring --import-keyring /etc/ceph/ceph.client.admin.keyring
```

```console
$ ceph-authtool /tmp/ceph.mon.keyring --import-keyring /var/lib/ceph/bootstrap-osd/ceph.keyring
```

```console
$ chown ceph:ceph /tmp/ceph.mon.keyring
```

```console
$ monmaptool --create --add localhost.localdomain 192.168.1.74 --fsid 9820a792-a2d8-42a0-a7e5-6e23a2bc9d69 /tmp/monmap
monmaptool: monmap file /tmp/monmap
monmaptool: set fsid to 9820a792-a2d8-42a0-a7e5-6e23a2bc9d69
monmaptool: writing epoch 0 to /tmp/monmap (1 monitors)
```

Next, create a default data directory for the montior:
```console
$ chown -R ceph:ceph /var/lib/ceph/
$ mkdir -p /var/lib/ceph/mon/ceph-localhost.localdomain
```
The format of the directory name is `clustername-hostname`, so in the our case
I'm using `ceph` as the cluster name and its is on my local machine which is
named `localhost.localdomain`.

Now, we need to populate the monitor daemon with the monitor map we generated
above and the keyring:
```console
$ sudo -u ceph ceph-mon --cluster ceph --mkfs -i localhost.localdomain --monmap /tmp/monmap --keyring /tmp/ceph.mon.keyring
```

#### Start the monitor
```console
$ ceph-mon -f --mon-data /var/lib/ceph/mon/ceph-localhost.localdomain/ --id localhost.localdomain --setuser root --setgroup root
```

Connect to the monitor using unix domain socket:
```console
sudo ceph --admin-daemon /var/run/ceph/ceph-mon.localhost.localdomain.asok version
{
    "version": "16.0.0-1852-g79c7ac53e6",
    "release": "pacific",
    "release_type": "dev"
}
```

Check the status of the cluster:
```console
$ sudo ceph -s
[sudo] password for danielbevenius: 
  cluster:
    id:     9820a792-a2d8-42a0-a7e5-6e23a2bc9d69
    health: HEALTH_WARN
            1 monitors have not enabled msgr2
 
  services:
    mon: 1 daemons, quorum localhost.localdomain (age 10s)
    mgr: no daemons active
    osd: 0 osds: 0 up, 0 in
 
  data:
    pools:   0 pools, 0 pgs
    objects: 0 objects, 0 B
    usage:   0 B used, 0 B / 0 B avail
    pgs:
```

Tail monitor logs:
```console
$ tail -f /var/log/ceph/ceph-mon.localhost.localdomain.log
```



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

```console
$ sudo ./cephadm bootstrap --mon-ip 192.168.1.74 --allow-fqdn-hostname
...
INFO:cephadm:Non-zero exit code 2 from /usr/bin/podman run --rm --net=host -e CONTAINER_IMAGE=docker.io/ceph/ceph:v15 -e NODE_NAME=localhost.localdomain -v /var/log/ceph/82caea9a-9fd3-11ea-b761-482ae34923e7:/var/log/ceph:z -v /tmp/ceph-tmphd_55e_u:/etc/ceph/ceph.client.admin.keyring:z -v /tmp/ceph-tmpgtca6lau:/etc/ceph/ceph.conf:z --entrypoint /usr/bin/ceph docker.io/ceph/ceph:v15 orch host add localhost.localdomain
INFO:cephadm:/usr/bin/ceph:stderr Error ENOENT: Failed to connect to localhost.localdomain (localhost.localdomain).  Check that the host is reachable and accepts connections using the cephadm SSH key
INFO:cephadm:/usr/bin/ceph:stderr you may want to run: 
INFO:cephadm:/usr/bin/ceph:stderr > ssh -F =(ceph cephadm get-ssh-config) -i =(ceph config-key get mgr/cephadm/ssh_identity_key) root@localhost.localdomain
Traceback (most recent call last):
  File "./cephadm", line 4614, in <module>
    r = args.func()
  File "./cephadm", line 1154, in _default_image
    return func()
  File "./cephadm", line 2584, in command_bootstrap
    cli(['orch', 'host', 'add', host])
  File "./cephadm", line 2441, in cli
    ).run(timeout=timeout)
  File "./cephadm", line 2174, in run
    self.run_cmd(), desc=self.entrypoint, timeout=timeout)
  File "./cephadm", line 837, in call_throws
    raise RuntimeError('Failed command: %s' % ' '.join(command))
RuntimeError: Failed command: /usr/bin/podman run --rm --net=host -e CONTAINER_IMAGE=docker.io/ceph/ceph:v15 -e NODE_NAME=localhost.localdomain -v /var/log/ceph/82caea9a-9fd3-11ea-b761-482ae34923e7:/var/log/ceph:z -v /tmp/ceph-tmphd_55e_u:/etc/ceph/ceph.client.admin.keyring:z -v /tmp/ceph-tmpgtca6lau:/etc/ceph/ceph.conf:z --entrypoint /usr/bin/ceph docker.io/ceph/ceph:v15 orch host add localhost.localdomain
```
```console
$ sudo ceph cephadm get-ssh-config
[sudo] password for danielbevenius: 
Host *
  User root
  StrictHostKeyChecking no
  UserKnownHostsFile /dev/null
  ConnectTimeout=30
$ sudo ceph config-key get mgr/cephadm/ssh_identity_key


$ ssh -F =(ceph cephadm get-ssh-config) -i =(ceph config-key get mgr/cephadm/ssh_identity_key) root@localhost.localdomain
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
