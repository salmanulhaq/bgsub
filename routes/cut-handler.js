var router = require('express').Router();
var fs = require('fs');
var bgsub = require('/usr/lib/bgsub');
var async = require('async');

router.post('/', function(request, response, next) {

	var params = request.body;

	var numThreads = params.num_threads || 2;

	var inputBasePath = process.env['HOME']+'/input/';
	var outputBasePath = process.env['HOME']+'/output/';
	
	if (!fs.existsSync(inputBasePath)) {
		response.status(501).json({status:'ERROR', message: 'input directory does not exist on the server'});
	}
	
	if (!fs.existsSync(outputBasePath)) fs.mkdirSync(outputBasePath);

	var inputFiles = fs.readdirSync(inputBasePath);
	var images = [];

	inputFiles.forEach(function(inputFile){
		if(fs.lstatSync(inputBasePath+inputFile).isFile()) {
			if(inputFile.indexOf('.png') != -1){
				images.push({input: inputBasePath+inputFile, output: outputBasePath+inputFile});
			} else {
				images.push({input: inputBasePath+inputFile, output: outputBasePath+inputFile+'.png'});
			}

		}
	});

	var totalProcessed = 0;
	var totalLatency = 0.0;

	async.eachLimit(images, numThreads,
	function(image, next) {
		bgsub.subtract(image.input, image.output, function(error, latency){
			if(error) {
				console.log(error);
				next();
			} else {
				totalProcessed += 1;
				totalLatency += parseFloat(latency);
				next();
			}
		});
	},
	function(error) {

		if(error) {
			response.status(501).json({status:'ERROR', message: error});
		} else {

			var result = {
				total_processed: totalProcessed,
				total_processing_time_ms: totalLatency.toFixed(2),
				average_processing_time_ms: (totalLatency/totalProcessed).toFixed(2),
				output_dir: outputBasePath
			};

			response.status(201).json(result);
		}

		return next(); 
	});

});

module.exports = router;