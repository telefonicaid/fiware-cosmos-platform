$(document).ready(function(){
	
	$('.combo .title').click(function () {
		$(this).parent().find('ul').toggle("slow");
		$('span',this).toggleClass('opened');
	});
	
	$(".combo li a").hover(function () {
	    $(this).parent().css('background','#2680CA');
	    $(this).css('color','#fff');
	  }, 
	  function () {
	    $(this).parent().css('background','#fff');
	     $(this).css('color','#2680CA');
	  });
	
	$('.box .tabs li span').click(function () {
		$(this).parent().find('ul').toggle("fast");
		$(this).parent().toggleClass('opened');
	});
});