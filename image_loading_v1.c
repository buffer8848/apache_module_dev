/*========================================================================*/
/*在IE上显示JPG格式的图片，输入路径如下：                                          */
/*      http://localhost/myimage?action=display&filename=1.jpg           */
/*打开事先设置好的默认路径下的图片。这段代码是最简单的方法利用html解析器，利用     */
/*内容生成器返回客户端<img src="url" mce_src="url">的HTML标签来达到这种效果。  */
/*auth：buffer                                                            */
/*time：21/07/09                                                          */
/*email: libinfei8848@163.com                                             */
/*========================================================================*/

#include <httpd.h>
#include <http_protocol.h>
#include <http_config.h>
#include <apr_errno.h>
#include <apr_general.h>
#include <http_log.h>
#include <apr_hash.h>
#include <apr_strings.h>

module AP_MODULE_DECLARE_DATA myimage_module;

static int myimage_handler(request_rec *r)
{
	int pointer = 0;  //用于解析URL的游码标尺 
	const char *root_path = "c://images//"; //用于保存图片位置的根目录
	const char *path = NULL; //用于保存获取图片的绝对路径

	//检查是否需要该模块执行
	if (!r->handler || strcmp(r->handler, "myimage")) 
	{
		return DECLINED;
	}
	//确认是以GET方法获取数据
	if ((r->method_number != M_GET) && (r->method_number != M_POST)) 
	{
		return HTTP_METHOD_NOT_ALLOWED;
	}

	ap_set_content_type(r, "text/html;charset=gb2312");
	ap_rputs("<!DOCTYPE HTML PUBLIC /"-//W3C//DTD HTML 4.01//EN/">/n", r);
	ap_rputs("<html><head><title>Apache Myimage Module</title></head>", r);
	ap_rputs("<body><center><h1>Look out!</h1>", r);

	//解析URL
	if( 0 != apr_strnatcmp(apr_pstrndup(r->pool, r->args, 7), "action="))
	{
		ap_rputs("<p><h1>Please check the URL of phone!</h1></p>/n", r);
		return OK;
	}
	pointer += 7;
	if(0 == apr_strnatcmp(apr_pstrndup(r->pool, (r->args)+pointer, 8), "display&"))
	{
		pointer += 8;
		if(0 == apr_strnatcmp(apr_pstrndup(r->pool, (r->args)+pointer, 9), "filename="))
		{
			pointer += 9;
			//path = apr_pstrcat(r->pool, "GET ", root_path, (r->args)+pointer, " HTTP/1.1.",  NULL);
			path = apr_pstrcat(r->pool, root_path, (r->args)+pointer, NULL);
			//r->the_request = path;   //改变请求中的第一行，改为请求一张图片
			ap_rputs("<p>This is the JPG you want!</p></center>", r);
			//Display JPG
			ap_rprintf(r, "<center><img src="%s" mce_src="%s"></center>", path);
			ap_rputs("<center>这是一种最简单的调用方法，利用浏览器的HTML自动解析功能！", r);
		}
		else
		{
			ap_rputs("<p><h1>Please check the URL of phone!</h1></p>/n", r);
			return OK;
		}
	}
	else if(0 == apr_strnatcmp(apr_pstrndup(r->pool, (r->args)+pointer, 8), "listfile"))
	{
		//这部分是需要显示目录下的文件列表，目前没有实现
		return OK;
	}
	else
	{
		ap_rputs("<p><h1>Please check the URL of phone!</h1></p>/n", r);
		return OK;
	}

	ap_rputs("</body></html>", r);

	return OK;
}

static void register_hooks(apr_pool_t *pool)
{
	ap_hook_handler(myimage_handler, NULL, NULL, APR_HOOK_MIDDLE);
}

module AP_MODULE_DECLARE_DATA myimage_module = 
{
	STANDARD20_MODULE_STUFF,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	register_hooks
};
