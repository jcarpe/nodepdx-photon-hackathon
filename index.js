var fs = require( 'fs' );
var Twitter = require('twitter');
var Particle = require('particle-api-js');
var particle = new Particle();

var token,
	deviceID;

var tweets = [
	'Somebody pushed my photon button at #NodePDX @LosantHQ ',
	'A person pushed my photon button at #NodePDX @LosantHQ '
];

var tweetIndex = 0;

var tweetClient = new Twitter({
	consumer_key: '',
	consumer_secret: '',
	access_token_key: '',
	access_token_secret: ''
});

fs.readFile('./count.txt', 'utf-8', (err, data) => {
  if (err) throw err;
  console.log( data );
  tweetIndex = data;
});

particle.login({
		username: '',
		password: ''
	})
	.then( ( data ) => {
		console.log( 'API call completed on promise resolve: ', data.body.access_token );
		token = data.body.access_token;
		getAllDevices();
	});

function getAllDevices() {
	particle.listDevices({
			auth: token 
		})
		.then( ( devices ) => {
			console.log( 'Devices:', devices );
			deviceID = devices.body[0].id;
			console.log( 'our device:', deviceID );
			listenToDevice();
		})
}

function listenToDevice() {
	particle.getEventStream({
			deviceId: deviceID,
			auth: token
		})
		.then( function( stream ) {
			stream.on( 'nodePDX-button-pushed', function( data ) {
				console.log( 'Event: ' + data );

				tweetClient.post( 'statuses/update', {status: tweetIndex + ' people have pushed my photon button at #NodePDX @LosantHQ'},
					function(error, tweet, response){
						if( error ) {
							console.log( error );
						}
						console.log( tweet.text, ' <-- went to twitter' );  // Tweet body.
						tweetIndex++;

						fs.writeFile('count.txt', tweetIndex, (err) => {
							if (err) throw err;
							console.log('It\'s saved!');
						});
						
					});
			});
		});
}