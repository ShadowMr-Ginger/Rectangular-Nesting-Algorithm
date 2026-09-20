using Backend.Native;

var builder = WebApplication.CreateBuilder(args);

// 固定监听 5088，保证 `dotnet run`（无论 launchSettings 是否生效）都在该端口。
builder.WebHost.UseUrls("http://localhost:5088");

builder.Services.AddControllers();

// 允许任意来源（前端开发服务器端口不固定）。
builder.Services.AddCors(options =>
{
    options.AddDefaultPolicy(policy =>
        policy.AllowAnyOrigin().AllowAnyMethod().AllowAnyHeader());
});

var app = builder.Build();

app.UseCors();
app.MapControllers();

app.Run();
