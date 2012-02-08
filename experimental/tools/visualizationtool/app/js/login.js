/**
 * @author DANIEL ESONO FERRER
 * @author JAVIER JIMENEZ GAZOL
 */

//Creation complete function
$().ready(function()
{
	// Validate log in form on keyup and submit
	$("#registerForm").validate({
		rules: {
			password: {
				required: true,
				minlength: 5
			},
			email: {
				required: true,
				email: true
			}
		},
		messages: {
			password: {
				required: "Please provide a password",
				minlength: "Your password must be at least 5 characters long"
			},
			email: "Please enter a valid email address",
		}			
	});
	
	
	// Validate forgot password form on keyup and submit
	$("#passwordForm").validate({
		rules: {
			email: {
				required: true,
				email: true
			}
		},
		messages: {
			email: "Please enter a valid email address",
		}			
	});

   
   //Function to hide the Forgot Password div and show Log in div
   $("#cancelbtn").click(function()
	{
     	$("#contentLogin").css("display", "block");
		$("#contentPassword").css("display", "none");
		document.getElementById("htitle").innerHTML = "Log in";	
   });
   
   
   //Function to show the Forgot Password div and hide Log in div
   $("#aforgot").click(function()
	{
     	$("#contentLogin").css("display", "none");
		$("#contentPassword").css("display", "block");
		document.getElementById("htitle").innerHTML = "Forgot Password";
	//	$("b").text("Forgot Password");
   });
});	
