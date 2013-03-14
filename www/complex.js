// Map related codes
var directionsDisplay = null;
var directionsService = new google.maps.DirectionsService();
var map, geocoder;
var oldDirections = [];
var currentDirections = null;
// Create our "tiny" marker icon
var gGreenIcon, gRedIcon;
var srcMarker, dstMarker, srcPos, dstPos;
var routeSteps = [];

function initGMap() {
  var myOptions = {
    zoom: 15,
    center: new google.maps.LatLng(40.4445,-79.957155),
    mapTypeId: google.maps.MapTypeId.ROADMAP
  }
  map = new google.maps.Map(document.getElementById("map_canvas"), myOptions);
  
  // Check for geolocation support
	if(navigator.geolocation) {
		// Use method getCurrentPosition to get coordinates
		navigator.geolocation.getCurrentPosition(function(position) {
				myOptions.center = new google.maps.LatLng(position.coords.latitude, position.coords.longitude);
				map = new google.maps.Map(document.getElementById("map_canvas"), myOptions);
			}, function(){});			
	}
	else
	{
		map = new google.maps.Map(document.getElementById("map_canvas"), myOptions);
	}
	
	geocoder = new google.maps.Geocoder(); 
	
	gGreenAIcon = new google.maps.MarkerImage(
    "http://maps.gstatic.com/mapfiles/markers2/marker_greenA.png",
    new google.maps.Size(20, 34),
    new google.maps.Point(0, 0),
    new google.maps.Point(6, 20));
  gGreenBIcon = new google.maps.MarkerImage(
    "http://maps.gstatic.com/mapfiles/markers2/marker_greenB.png",
    new google.maps.Size(20, 34),
    new google.maps.Point(0, 0),
    new google.maps.Point(6, 20));
	
  directionsDisplay = new google.maps.DirectionsRenderer({
      'map': map,
      'preserveViewport': false,
      'draggable': false
  });
  directionsDisplay.setPanel(document.getElementById("directions_panel"));

  google.maps.event.addListener(directionsDisplay, 'directions_changed',
    function() {
      if (currentDirections) {
        oldDirections.push(currentDirections);
      }
      currentDirections = directionsDisplay.getDirections();
    });
  
  // Source auto complete
  $(function() {
       $("#route_source").autocomplete({
       
         source: function(request, response) {

		         if (geocoder == null){
		           geocoder = new google.maps.Geocoder();
		         }
             geocoder.geocode( {'address': request.term }, function(results, status) {
               if (status == google.maps.GeocoderStatus.OK) {

                  var searchLoc = results[0].geometry.location;
               		var lat = results[0].geometry.location.lat();
                  var lng = results[0].geometry.location.lng();
                  var latlng = new google.maps.LatLng(lat, lng);
                  var bounds = results[0].geometry.bounds;

                  geocoder.geocode({'latLng': latlng}, function(results1, status1) {
                      if (status1 == google.maps.GeocoderStatus.OK) {
                        if (results1[1]) {
                         response($.map(results1, function(loc) {
                        return {
                            label  : loc.formatted_address,
                            value  : loc.formatted_address,
                            bounds : loc.geometry.bounds,
                            position	: new google.maps.LatLng(loc.geometry.location.lat(), 																					loc.geometry.location.lng()) 
                          }
                        }));
                        }
                      }
                    });
            }
            });
         },
         select: function(event,ui){
			      var position = ui.item.position;
			      var label = ui.item.label;
			      var bounds = ui.item.bounds;
			
						srcPos =  position;

						if (bounds){
							if(dstPos) bounds.extend(dstPos);
			       	map.fitBounds(bounds);
			      }
			      map.panTo(position);
			      
			      if(srcMarker)
			      {
			      	srcMarker.setMap(null);
			      	srcMarker = null;
			      }
			      srcMarker = new google.maps.Marker({
			        position: position,
			        icon: gGreenAIcon, shadow: null, map: map});				      
         }
       });
   });
   
   
   // Destination auto complete
   $(function() {
       $("#route_destination").autocomplete({
       
         source: function(request, response) {

		         if (geocoder == null){
		           geocoder = new google.maps.Geocoder();
		         }
             geocoder.geocode( {'address': request.term }, function(results, status) {
               if (status == google.maps.GeocoderStatus.OK) {

                  var searchLoc = results[0].geometry.location;
               var lat = results[0].geometry.location.lat();
                  var lng = results[0].geometry.location.lng();
                  var latlng = new google.maps.LatLng(lat, lng);
                  var bounds = results[0].geometry.bounds;

                  geocoder.geocode({'latLng': latlng}, function(results1, status1) {
                      if (status1 == google.maps.GeocoderStatus.OK) {
                        if (results1[1]) {
                         response($.map(results1, function(loc) {
                        return {
                            label  : loc.formatted_address,
                            value  : loc.formatted_address,
                            bounds   : loc.geometry.bounds,
                            position	: new google.maps.LatLng(loc.geometry.location.lat(), 																					loc.geometry.location.lng())
                          }
                        }));
                        }
                      }
                    });
            }
            });
         },
         select: function(event,ui){
			      var position = ui.item.position;
			      var label = ui.item.label;
			      var bounds = ui.item.bounds;
			
						dstPos = position;
						if (bounds){
							if(srcPos) bounds.extend(srcPos);
			       	map.fitBounds(bounds);
			      }
			      map.panTo(position);
			      
			      if(dstMarker)
			      {
			      	dstMarker.setMap(null);
			      	dstMarker = null;
			      }
			      dstMarker = new google.maps.Marker({
			        position: position,
			        icon: gGreenBIcon, shadow: null, map: map});	
         }
       });
   });
  
}

function calcRoute() {
	var start = $("#route_source").val();
  var end = $("#route_destination").val();
  
  var request = {
      origin:start,
      destination:end,
      travelMode: google.maps.DirectionsTravelMode.WALKING
  };
  directionsService.route(request, function(response, status) {
    if (status == google.maps.DirectionsStatus.OK) {
      if(srcMarker)
      {
      	srcMarker.setMap(null);
      	srcMarker = null;
      }
      
      if(dstMarker)
      {
      	dstMarker.setMap(null);
      	dstMarker = null;
      }
      
      directionsDisplay.setDirections(response);
      
      routeSteps = [];
      if(response.routes.length > 0 
      		&& response.routes[0].legs.length > 0)
      {
      	for( var i = 0; i < response.routes[0].legs.length; i++ )
      	{
      		for( var j = 0; j < response.routes[0].legs[i].steps.length; j++)
      		{
      			// Strip HTML tags
      			var tmp = document.createElement("DIV");
				   	tmp.innerHTML = response.routes[0].legs[i].steps[j].instructions;
				   	
      			routeSteps.push(tmp.textContent || tmp.innerText);
      		}
      	}
      }
      
      // Help info
			$("#help_info_panel").html('<br><span style="color:green;"><h5>Say <strong>"Start Route"</strong> or "Set Destination" or "Set Source" or "End Route" Command.</h4></span>');

   }
   else
   {
   		if( typeof window.tsWebProxyObject != 'undefined' )
   		{
   			tsWebProxyObject.speak("Cannot find route for this reqeust.");
   		}
   		
   		$("#directions_panel").html('<br><span style="color:red;"><h5>Cannot find route for this reqeust.</h4></span>');
   }
  });
}

function clearRoute() {
		$("#route_source").val("");
		$("#route_destination").val("");
		
		srcPos = dstPos = null;
		
		if(srcMarker)
    {
    	srcMarker.setMap(null);
    	srcMarker = null;
    }
    
    if(dstMarker)
    {
    	dstMarker.setMap(null);
    	dstMarker = null;
    }
    
    if( directionsDisplay )
    {
    	directionsDisplay.setMap(null);
    }
    
    $("#directions_panel").slideUp("fast");
}

function geoCode(addr, is_src)
{
	 if (geocoder == null){
     geocoder = new google.maps.Geocoder();
   }
   
   geocoder.geocode( {'address': addr }, function(results, status) {
     if (status == google.maps.GeocoderStatus.OK) {

        var searchLoc = results[0].geometry.location;
     		var lat = results[0].geometry.location.lat();
        var lng = results[0].geometry.location.lng();
        var latlng = new google.maps.LatLng(lat, lng);
        var bounds = results[0].geometry.bounds;
				
				if(is_src)
					srcPos = latlng;
				else
					dstPos = latlng;
					
				if (bounds){
						if(srcPos) bounds.extend(srcPos);
						if(dstPos) bounds.extend(dstPos);
		       	map.fitBounds(bounds);
		    }
		    map.panTo(latlng);
		    
		    if(is_src)
	      {
	      	if(srcMarker)
	      	{
		      	srcMarker.setMap(null);
		      	srcMarker = null;
	      	}
	      	srcMarker = new google.maps.Marker({
								        position: latlng,
								        icon: gGreenAIcon, shadow: null, map: map});
	      }
	         
	      if(!is_src)
	      {
	      	if(dstMarker)
	      	{
		      	dstMarker.setMap(null);
		      	dstMarker = null;
	      	}
	      	dstMarker = new google.maps.Marker({
								        position: latlng,
								        icon: gGreenBIcon, shadow: null, map: map});	
	      }
	      
      }
   });     
}

// Layout codes
var pageLayout;

$(document).ready(function(){
	// create page layout
	pageLayout = $('body').layout({
		defaults: {
		}
	,	north: {
			size:					"auto"
		,	spacing_open:			0
		,	closable:				false
		,	resizable:				false
		}
	,	west: {
			size:					345
		,	spacing_closed:			22
		,	togglerLength_closed:	140
		,	togglerAlign_closed:	"top"
		,	togglerContent_closed:	""
		,	togglerTip_closed:		"Open & Pin Contents"
		,	sliderTip:				"Slide Open Contents"
		,	slideTrigger_open:		"mouseover"
		, closable:					false
		}
	});

	initGMap();
	
	$("#get_direction").bind('click', calcRoute);
	$("#clear_addr").bind('click', clearRoute);
	
	// Help info
	$("#help_info_panel").html('<br><span style="color:green;"><h5>Say "Set Destination" Command.</h4></span>');
	
	try {
		if( window.tsWebProxyObject )
		{
			tsWebProxyObject.SetSource.connect(setSource);
			tsWebProxyObject.SetDestination.connect(setDestination);
			tsWebProxyObject.GetPath.connect(calcRoute);
			tsWebProxyObject.RouteStart.connect(startRoute);
			tsWebProxyObject.RouteStop.connect(stopRoute);
			tsWebProxyObject.UNRECOGNIZED.connect(onError);
		}
	}
	catch(e) {
	}
	
});

function setSource(bldg, src)
{
	 $("#route_source").val(src);
   //$("#route_source").autocomplete('search', src);
   geoCode(src, true);
   $("#src_bldg_name").text(bldg);
   
   // Help info
	$("#help_info_panel").html('<br><span style="color:green;"><h5>Say <strong>"Get Path"</strong> or "Set Source" or "End Route" Command.</h4></span>');

}

function setDestination(bldg, dst)
{
	$("#route_destination").val(dst);
	//$("#route_destination").autocomplete('search', dst);
	geoCode(dst, false);
	$("#dst_bldg_name").text(bldg);
	
	// Help info
	$("#help_info_panel").html('<br><span style="color:green;"><h5>Say "Set Destination" or "Set Source" or "End Route" Command.</h4></span>');

}

function startRoute()
{
	for( var i = 0; i < routeSteps.length; i++)
	{
		tsWebProxyObject.speak(routeSteps[i]);
	}
	
	// Help info
	$("#help_info_panel").html('<br><span style="color:green;"><h5>Say <strong>"End Route"</strong> or "Start Route" Command.</h4></span>');

}

function stopRoute()
{
	$("#route_source").val("");
	$("#route_destination").val("");
	
	srcPos = dstPos = null;
	if(srcMarker)
  {
  	srcMarker.setMap(null);
  	srcMarker = null;
  }
  
  if(dstMarker)
  {
  	dstMarker.setMap(null);
  	dstMarker = null;
  }
  
  if( directionsDisplay )
  {
  	directionsDisplay.setMap(null);
  }
  
  $("#src_bldg_name").text("Speak a Building Name of PITT. Command: set source");
  $("#dst_bldg_name").text("Speak a Building Name of PITT. Command: set destination");
  
  // Help info
	$("#help_info_panel").html('<br><span style="color:green;"><h5>Say "Set Destination" Command.</h4></span>');
	
  $("#directions_panel").slideUp("fast");
  
  routeSteps = [];
  
}

function onError(err)
{
	$("#").text(err);
}