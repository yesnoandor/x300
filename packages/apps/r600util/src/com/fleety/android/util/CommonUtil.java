package com.fleety.android.util;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * @author WILLKINSON
 * @since 2011-8-18
 * @doc 通用工具类
 */
public class CommonUtil
{
    /**
     * 将0x00的格式字符串转换成byte型数据
     * 
     * @param str
     * @return
     */
    public static byte OX2Byte(String str)
    {
        try
        {
            String str1 = str.toLowerCase().replaceAll("0x", "");
            return (byte) Short.parseShort(str1, 16);
        }
        catch (Exception e)
        {
            e.printStackTrace();
            return 0;
        }
    }

    /**
     * 将0x0000的格式字符串转换成short型数据
     * 
     * @param str
     * @return
     */
    public static short OX2Short(String str)
    {
        String str1 = str.toLowerCase().replaceAll("0x", "");
        return (short) Integer.parseInt(str1, 16);
    }

    public static int OX2Int(String str)
    {
        String str1 = str.toLowerCase().replaceAll("0x", "");
        return Integer.parseInt(str1, 16);
    }

    /**
     * 判断字符串是否是NULL或者是空字符串
     * 
     * @param str
     * @return
     */
    public static boolean isBlankString(String str)
    {
        return str == null || "".equals(str);
    }

    /**
     * 将无符号型的byte转换成有符号型的byte
     * 
     * @param value
     *            无符号型的数值
     * @return 有符号型的byte
     */
    public static byte unsignToSignByte(short value)
    {
        byte returnValue;
        if (value > 128)
        {
            returnValue = (byte) (128 - value);
        }
        else returnValue = (byte) value;
        return returnValue;
    }

    /**
     * @param value
     *            有符号型的数值
     * @return 无符号型的数值
     */
    public static int toUnsign(byte value)
    {
        int temp = 0;
        if (value < 0)
        {
            temp = value + 256;
        }
        else
        {
            temp = value;
        }
        return temp;
    }

    /**
     * @param value
     *            无符号型的数值
     * @return 有符号型的数值
     */
    public static byte toSignByte(int value)
    {
        byte temp = 0;
        if (value > Byte.MAX_VALUE)
        {
            temp = (byte) (value - 256);
        }
        else
        {
            temp = (byte) value;
        }
        return temp;
    }

    /**
     * @param value
     *            有符号型的数值
     * @return 无符号型的数值
     */
    public static int toUnsign(short value)
    {
        int temp = 0;
        if (value < 0)
        {
            temp = value + 65536;
        }
        else
        {
            temp = value;
        }
        return temp;
    }

    /**
     * @param value
     *            无符号型的数值
     * @return 有符号型的数值
     */
    public static short toSignShort(int value)
    {
        short temp = 0;
        if (value > Short.MAX_VALUE)
        {
            temp = (short) (value - 65536);
        }
        else
        {
            temp = (short) value;
        }
        return temp;
    }

    /**
     * @param value
     *            有符号型的数值
     * @return 无符号型的数值
     */
    public static long toUnsign(int value)
    {
        long temp = 0;
        if (value < 0)
        {
            temp = 4294967296l + value;
        }
        else
        {
            temp = value;
        }
        return temp;
    }

    /**
     * @param value
     *            无符号型的数值
     * @return 有符号型的数值
     */
    public static int toSignInt(long value)
    {
        int temp = 0;
        if (value > Integer.MAX_VALUE)
        {
            temp = (int) (value - 4294967296l);
        }
        else
        {
            temp = (int) value;
        }
        return temp;
    }

    /**
     * 将时间转换为YYHHDDhhmmss格式
     * 
     * @param time
     *            时间
     * @return 时间所代表的字符串
     */
    public static String toYYMMDDhhmmss(long time)
    {

        return toString(time, "yyMMddHHmmss");
    }

    /**
     * format to yyMMddHHmmss
     * 
     * @param time
     * @return
     */
    public static String toYYMMDDhhmmss(Date time)
    {
        SimpleDateFormat format = new SimpleDateFormat("yyMMddHHmmss");
        return format.format(time);
    }

    /**
     * format to yyyy-MM-dd HH:mm:ss
     * 
     * @param time
     * @return
     */
    public static String toYYYYMMDDhhmmss(Date time)
    {
        SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        return format.format(time);
    }

    /**
     * 将消息中的 BCD码转为 String
     */
    public static String BCDToString(Object value)
    {
        byte[] s = (byte[]) value;
        StringBuffer buff = new StringBuffer();
        for (byte i : s)
        {
            buff.append(i);
        }
        return buff.toString();
    }

    /**
     * 将字符串根据长度要求,如果超出长度从后面截取掉,如果长度不足在前面补0
     * @param source
     * @param length bcd长度
     * @return
     */
    public static String createBCDString(String source, int length)
    {
        String result = null;
        StringBuffer buffer = new StringBuffer(source);
        if (buffer.length() < length)
        {
            int diff = length - buffer.length();
            for (int i = 0; i < diff; i++)
            {
                buffer.insert(0, "0");
            }
            result = buffer.toString();
        }
        else if (buffer.length() >= length)
        {
            result = buffer.substring(0, length);
        }
        return result;
    }

    /**
     * 将时间转换为指定的格式
     * 
     * @param time
     *            时间
     * @param pattern
     *            格式
     * @return 时间所代表的字符串
     */
    public static String toString(long time, String pattern)
    {
        return new SimpleDateFormat(pattern).format(new Date(time));
    }

    /**
     * 
     * @param time
     *            时间字符串
     * @param pattern
     *            格式
     * @return 时间所代表的long值
     * @throws ParseException
     */
    public static long toLong(String time, String pattern)
            throws ParseException
    {
        return new SimpleDateFormat(pattern).parse(time).getTime();
    }
}
