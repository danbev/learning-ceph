const buildType = process.config.target_defaults ?
  process.config.target_defaults.default_configuration :
  'Release';
const ceph_napi = require(`./build/${buildType}/cephnapi.node`);

console.log(`librados version: ${ceph_napi.version()}`);
module.exports = ceph_napi;

const username = "client.admin"
const clustername = "ceph"
ceph_napi.init(username, clustername, (value) => {
  console.log('init callback: ', value);
});
