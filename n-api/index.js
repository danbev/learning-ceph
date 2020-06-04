const buildType = process.config.target_defaults ?
  process.config.target_defaults.default_configuration :
  'Release';

const { Ceph } = require(`./build/${buildType}/cephnapi.node`);

const ceph = new Ceph({
  ceph_conf: '/home/danielbevenius/work/ceph/ceph/build/ceph.conf'
});
console.log(`librados version: ${ceph.version()}`);

const username = "client.admin"
const clustername = "ceph"
ceph.connect(username, clustername, (value) => {
  console.log('init callback: ', value);
});
