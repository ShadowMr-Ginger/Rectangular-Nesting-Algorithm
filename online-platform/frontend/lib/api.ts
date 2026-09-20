import type { Strings } from "./i18n";

function extractMessage(data: unknown, fallback: string): string {
  if (data && typeof data === "object") {
    const obj = data as Record<string, unknown>;
    if (typeof obj.message === "string" && obj.message.trim()) {
      let msg = obj.message;
      const errors = obj.errors;
      if (Array.isArray(errors) && errors.length > 0) {
        msg += "：" + errors.map((e) => (typeof e === "string" ? e : JSON.stringify(e))).join("；");
      } else if (errors && typeof errors === "object") {
        const detail = Object.entries(errors as Record<string, unknown>)
          .map(([k, v]) => `${k}: ${String(v)}`)
          .join("；");
        if (detail) msg += "：" + detail;
      }
      return msg;
    }
  }
  return fallback;
}

export async function apiPost<T>(path: string, body: unknown, s?: Strings): Promise<T> {
  let res: Response;
  try {
    res = await fetch(path, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(body),
    });
  } catch {
    throw new Error(s?.netErr ?? "无法连接后端服务，请确认 API（http://localhost:5088）已启动。");
  }

  let data: unknown = null;
  const text = await res.text();
  if (text) {
    try {
      data = JSON.parse(text);
    } catch {
      data = null;
    }
  }

  if (!res.ok) {
    const fallback = s?.httpErr(res.status) ?? `请求失败（HTTP ${res.status}），请稍后重试。`;
    throw new Error(extractMessage(data, fallback));
  }
  return data as T;
}
