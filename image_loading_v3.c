/*========================================================================*/
/*在IE上显示JPG格式的图片，输入路径如下：                                          */
/*      http://localhost/myimage?action=display&filename=1.jpg            */
/*打开事先设置好的默认路径下的图片。这个方案是利用Apache自己的存储结构来实         */
/*现的，brigade可以认为是输入和输出之间的逻辑桥梁，将图片数据通过这个桥梁          */
/*传输到请求的下一个过滤器，然后再输出到浏览器。                                    */
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
	apr_file_t *fp = NULL;
	apr_status_t rv;
	apr_size_t size;  //用于标示文件的大小
	apr_finfo_t info; //用于获取文件句柄的详细信息
	apr_bucket *b;
	apr_bucket_brigade *bb;

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

	/*这个地方设置的是image/jpg，那么就不应该用ap_rputs等函数向里面写入任何数据，
	*这样会干扰图片的数据，造成浏览器显示乱码。如果使用的是text/html的话可以。*/
	ap_set_content_type(r, "image/jpg;charset=gb2312");
	//ap_rputs("<!DOCTYPE HTML PUBLIC /"-//W3C//DTD HTML 4.01//EN/">/n", r);
	//ap_rputs("<html><head><title>Apache Myimage Module</title></head>", r);
	//ap_rputs("<body><center><h1>Look out!</h1>", r);

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
			path = apr_pstrcat(r->pool, root_path, (r->args)+pointer, NULL);
			//ap_rputs("<p>This is the JPG you want!</p></center>", r);
			//Display JPG
			rv = apr_file_open(&fp, path, APR_READ, APR_OS_DEFAULT, r->pool);
			if (rv != APR_SUCCESS)
			{         
				//ap_rputs("<p><center>file permissions deny server access</p>/n", r);

				return OK;     
			}
			//获取文件句柄的详细信息
			apr_stat(&info,path,APR_FINFO_SIZE,r->pool);
			size=(apr_size_t)info.size;
			//利用桥梁这种存储类型来传输数据到下一个过滤器
			bb = apr_brigade_create(r->pool, r->connection->bucket_alloc);
			b=apr_bucket_file_create(fp,0,size,r->pool,r->connection->bucket_alloc);
			ap_set_content_length(r,size);
			APR_BRIGADE_INSERT_TAIL(bb,b);
			ap_pass_brigade(r->output_filters,bb);

			apr_file_close(fp);
		}
		else
		{
			//ap_rputs("<p><h1>Please check the URL of phone!</h1></p>/n", r);

			return OK;
		}
	}
	else if(0 == apr_strnatcmp(apr_pstrndup(r->pool, (r->args)+pointer, 8), "listfile"))
	{
		//显示目录下的图片列表，目前还没有实现
		return OK;
	}
	else
	{
		//ap_rputs("<p><h1>Please check the URL of phone!</h1></p>/n", r);

		return OK;
	}

	//ap_rputs("</body></html>", r);

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
