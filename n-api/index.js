const ceph_napi = require('./build/Release/cephnapi.node');

console.log(`librados version: ${ceph_napi.version()}`);
module.exports = ceph_napi;
const username = "client.admin"
const clustername = "ceph"
ceph_napi.init(username, clustername);
//ceph_napi.init(username);
