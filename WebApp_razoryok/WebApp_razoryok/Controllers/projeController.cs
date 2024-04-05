using Microsoft.AspNetCore.Mvc;

namespace WebApp_razoryok.Controllers
{
    public class projeController : Controller
    {
        public IActionResult Index()
        {
            return View();
        }
    }
}
