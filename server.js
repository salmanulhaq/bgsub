var express = require('express');
var cors = require('cors');
var bodyParser = require('body-parser');

var app = express();

app.use(bodyParser.json());
app.use(cors());

app.use('/cut', require('./routes/cut-handler'));

app.use(function(error, request, response, next) {
  response.status(error.status || 500);
  response.send();
  console.log(error);
  next();
});

var server = app.listen(4000, function() {
	console.log('cut server started. Listening on port 8080');
});