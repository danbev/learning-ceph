const AWS = require('aws-sdk');
const util = require('util');

AWS.config.update({
  apiVersion: '2006-03-01',
  region: 'default',
  endpoint: 'http://localhost:8000',
  accessKeyId: '0555b35654ad1656d804',
  secretAccessKey: 'h7GhxuBLTrlhVUyxSPUKUV8r/2EI4ngqJxD7iBdBYLhwluN30JaT3Q==',
  signatureVersion: 'v4',
  s3ForcePathStyle: true,
  sslEnabled: 'false',
});

const s3 = new AWS.S3();
const bucketName = 'samplebucket';
const key = 'somekey';

s3.createBucket({ Bucket: bucketName}, function(err, data) {
  if (err) {
    console.log(`Error: ${err}`);
  } else {
    console.log(`Successfully created bucket: ${bucketName}, data=${util.inspect(data)}`);
  }
});

const objectParams = {Bucket: bucketName, Key: key, Body: 'testing'};
const uploadPromise = s3.putObject(objectParams).promise();
uploadPromise.then(
  function(data) {
    console.log(`Successfully uploaded data to ${bucketName}/${key}`);
  }).catch(
  function(err) {
    console.error(err, err.stack);
});
