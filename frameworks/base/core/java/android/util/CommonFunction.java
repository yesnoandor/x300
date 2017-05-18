package android.util;

import java.text.SimpleDateFormat; 
import java.util.Date; 

public abstract class CommonFunction {

public static String getFileLineMethod() { 
	StackTraceElement traceElement = ((new Exception()).getStackTrace())[1]; 
	StringBuffer toStringBuffer = new StringBuffer("[").append( 
	traceElement.getFileName()).append(" | ").append( 
	traceElement.getLineNumber()).append(" | ").append( 
	traceElement.getMethodName()).append("]");

	return toStringBuffer.toString(); 
} 


public static String _FILE_() { 
	StackTraceElement traceElement = ((new Exception()).getStackTrace())[1]; 
	return traceElement.getFileName(); 
} 

 
public static String _FUNC_() { 
	StackTraceElement traceElement = ((new Exception()).getStackTrace())[1]; 
	return traceElement.getMethodName(); 
} 

public static int _LINE_() { 
	StackTraceElement traceElement = ((new Exception()).getStackTrace())[1]; 
	return traceElement.getLineNumber(); 
}


public static String _TIME_() { 
	Date now = new Date(); 
	SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS"); 
	return sdf.format(now); 
}
 
} 
