import type { Metadata } from "next";
import { headers } from "next/headers";
import "./globals.css";

/** 根据请求头 Accept-Language 决定标题语言：zh* → 中文，其余 → 英文 */
export async function generateMetadata(): Promise<Metadata> {
  const h = await headers();
  const acceptLang = (h.get("accept-language") ?? "").toLowerCase();
  const zh = acceptLang.startsWith("zh");
  return {
    title: zh
      ? "矩形件套料算法 · 在线运算演示平台"
      : "Rectangle Nesting Algorithm · Online Demo Platform",
    description: zh
      ? "矩形件套料算法的在线运算演示平台"
      : "Online demo platform for the rectangle nesting algorithm",
  };
}

export default function RootLayout({ children }: LayoutProps<"/">) {
  return (
    <html lang="zh-CN">
      <body>{children}</body>
    </html>
  );
}
