const ceph_napi = require('./build/Release/cephnapi.node');

console.log('ceph_napi:', ceph_napi);
console.log(`version: ${ceph_napi.version()}`);
module.exports = ceph_napi;
