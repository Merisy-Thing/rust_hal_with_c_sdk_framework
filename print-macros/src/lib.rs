extern crate proc_macro;

use core::mem;
use proc_macro::TokenStream;
use std::cmp::Ordering;
use std::borrow::Cow;

use proc_macro2::Span;
use quote::quote;
use syn::{
    parse::{self, Parse, ParseStream},
    parse_macro_input,
    punctuated::Punctuated,
    spanned::Spanned,
    Expr, ExprLit, LitStr, Token, Ident,
};

#[proc_macro]
pub fn print(input: TokenStream) -> TokenStream {
    write(input, false)
}

#[proc_macro]
pub fn println(input: TokenStream) -> TokenStream {
    write(input, true)
}

fn write(input: TokenStream, newline: bool) -> TokenStream {
    let input = parse_macro_input!(input as Input);
    let literal = input.literal;

    let mut format = literal.value();
    if newline {
        format.push('\n');
    }
    let pieces = match parse(&format, literal.span()) {
        Err(e) => return e.to_compile_error().into(),
        Ok(pieces) => pieces,
    };

    let required_args = pieces.iter().filter(|piece| !piece.is_str()).count();
    let supplied_args = input.args.len();
    match supplied_args.cmp(&required_args) {
        Ordering::Less => {
            return parse::Error::new(
                literal.span(),
                &format!(
                    "format string requires {} arguments but {} {} supplied",
                    required_args,
                    supplied_args,
                    if supplied_args == 1 { "was" } else { "were" }
                ),
            )
            .to_compile_error()
            .into();
        }
        Ordering::Greater => {
            return parse::Error::new(
                input.args[required_args].span(),
                //"argument never used".to_string(),
                &format!(
                    "format string requires {} arguments but {} {} supplied",
                    required_args,
                    supplied_args,
                    if supplied_args == 1 { "was" } else { "were" }
                ),
            )
            .to_compile_error()
            .into();
        }
        Ordering::Equal => {}
    }

    let mut args = vec![];
    let mut exprs = vec![];
    let mut dbgs = vec![];
    let mut i = 0;
    let mut format = String::new();
    for piece in pieces {
        if let Piece::Str(s) = piece {
            format.push_str(&s.to_string());
        } else {
            let arg = &input.args[i];
            i += 1;
            args.push(quote!(&(#arg)));

            match piece {
                Piece::Percent(pct) => {
                    match pct {
                        PCT::Int => {
                            exprs.push(quote!( #arg ));
                        }
                        PCT::Ch => {
                            exprs.push(quote!( #arg ));
                        }
                        PCT::Str => {
                            exprs.push(quote!(  (*#arg).as_ptr() as *const _ as InvokeParam, (*#arg).len() ));
                        }
                        PCT::Array => {
                            exprs.push(quote!(  (*#arg).as_ptr() as *const _ as InvokeParam, (*#arg).len() ));
                        }
                        PCT::Hex => {
                            exprs.push(quote!(  #arg ));
                        }
                    }
                }
                Piece::Display => {
                    let mut ok = false;
                    let mut err_msg = "".to_string();
                    let mut lable = "";
                    match arg {
                        Expr::Array(_) => {
                            lable = "%Y";
                            exprs.push(
                                quote!(  #arg.as_ptr() as *const _ as InvokeParam, #arg.len() ),
                            );
                            ok = true;
                        }
                        Expr::Reference(ref_exp) => {
                            let ref_expr = &*ref_exp.expr;
                            match ref_expr {
                                Expr::Lit(lit) => match fn_match_lit(lit, arg) {
                                    Ok((l, e)) => {
                                        lable = l;
                                        exprs.push(e);
                                        ok = true;
                                    }
                                    Err(msg) => err_msg = msg.to_string(),
                                },
                                Expr::Array(_) => {
                                    lable = "%Y";
                                    exprs.push(quote!(  #arg.as_ptr() as *const _ as InvokeParam, #arg.len() ));
                                    ok = true;
                                }
                                Expr::Path(path_expr) => {
                                    let first = path_expr.path.segments.first();
                                    if let Some(segment) = first {
                                        lable = "%Y";
                                        exprs.push(quote!( #segment.as_ptr() as *const _ as InvokeParam, #segment.len() ));
                                        ok = true;
                                    } else {
                                        err_msg = "Path has no segments".to_string();
                                    }
                                }
                                _ => {
                                    err_msg = map_expr_error(ref_expr);
                                    err_msg.push_str(" -> in match Expr::Reference");
                                }
                            }
                        }
                        Expr::Path(path_expr) => {
                            let first = path_expr.path.segments.first();
                            if let Some(segment) = first {
                                lable = "%d";
                                exprs.push(quote!( #segment ));
                                ok = true;
                            } else {
                                err_msg = "Path has no segments".to_string();
                            }
                        }
                        Expr::Lit(lit) => match fn_match_lit(lit, arg) {
                            Ok((l, e)) => {
                                lable = l;
                                exprs.push(e);
                                ok = true;
                            }
                            Err(msg) => err_msg = msg.to_string(),
                        },
                        Expr::Call(_) => {
                            lable = "%d";
                            exprs.push(quote!( #arg ));
                            ok = true;
                        }
                        Expr::MethodCall(_) => {
                            lable = "%d";
                            exprs.push(quote!( #arg ));
                            ok = true;
                        }
                        _ => {
                            err_msg = map_expr_error(arg);
                            err_msg.push_str(" -> in match arg");
                        }
                    }
                    if !ok {
                        return parse::Error::new(arg.span(), err_msg.to_string())
                            .to_compile_error()
                            .into();
                    }

                    format.push_str(lable);
                }
                Piece::Debug => {
                    let buf_x = mk_name_with_idx("buf", i);
                    let str_x = mk_name_with_idx("str", i);

                    dbgs.push(quote!(
                        let mut #buf_x = [0u8; 256];
                        let #str_x = format::fmt_to_buf(
                            &mut #buf_x,
                            format_args!("{:?}", #arg),
                        ).unwrap_or("format error");
                    ));
                    format.push_str("%S");
                    exprs.push(quote!(  #str_x.as_ptr() as *const _ as InvokeParam, #str_x.len() ));
                }
                Piece::Hex {
                    upper_case,
                    // pad_char,
                    // pad_length,
                    // prefix,
                } => {
                    format.push_str(if upper_case { "%X" } else { "%x" });
                    exprs.push(quote!( #arg ));
                }
                Piece::Str(_) => unreachable!(),
            }
        }
    }

    quote!(
        unsafe {
            use embedded_c_sdk_bind_hal::*;
            
            #(#dbgs )*
            ll_invoke( INVOKE_ID_LOG_PRINT, concat!(#format, "\0").as_ptr() as *const _ as InvokeParam, #(#exprs as InvokeParam, )* )
        }
    )
    .into()
}

fn fn_match_lit<'a>(
    lit: &ExprLit,
    arg: &Expr,
) -> Result<(&'a str, proc_macro2::TokenStream), &'a str> {
    let mut ok = false;
    let mut err_msg = "";
    let mut lable = "";
    let mut expr = proc_macro2::TokenStream::new();
    match &lit.lit {
        syn::Lit::Str(_) | syn::Lit::ByteStr(_) => {
            lable = "%S";
            expr = quote!(  #arg.as_ptr() as *const _ as InvokeParam, #arg.len() );
            ok = true;
        }
        syn::Lit::CStr(_) => {
            lable = "%s";
            expr = quote!(  #arg.as_ptr() as *const _ );
            ok = true;
        }
        syn::Lit::Byte(_) => {
            lable = "%d";
            expr = quote!(  #arg );
            ok = true;
        }
        syn::Lit::Char(_) => {
            lable = "%c";
            expr = quote!(  #arg );
            ok = true;
        }
        syn::Lit::Int(_) => {
            lable = "%d";
            expr = quote!(  #arg );
            ok = true;
        }
        syn::Lit::Float(_) => {
            err_msg = "Float types not supported yet";
        }
        syn::Lit::Bool(lit_bool) => {
            lable = "%s";
            if lit_bool.value() {
                expr = quote!("true\0".as_ptr() as *const _).into();
            } else {
                expr = quote!("false\0".as_ptr() as *const _).into();
            }
            ok = true;
        }
        _ => {
            err_msg = "Unsupported Lit parameter types";
        }
    }
    if ok {
        Ok((lable, expr))
    } else {
        Err(err_msg)
    }
}

struct Input {
    literal: LitStr,
    _comma: Option<Token![,]>,
    args: Punctuated<Expr, Token![,]>,
}

impl Parse for Input {
    fn parse(input: ParseStream) -> parse::Result<Self> {
        let literal = input.parse()?;

        if input.is_empty() {
            Ok(Input {
                literal,
                _comma: None,
                args: Punctuated::new(),
            })
        } else {
            Ok(Input {
                literal,
                _comma: input.parse()?,
                args: Punctuated::parse_terminated(input)?,
            })
        }
    }
}

#[derive(Debug, PartialEq)]
enum PCT {
    Int,
    Ch,
    Str,
    Hex,
    Array,
}

#[derive(Debug, PartialEq)]
enum Piece<'a> {
    Debug,
    Display,
    Str(Cow<'a, str>),
    Percent(PCT),
    Hex {
        upper_case: bool,
        // pad_char: u8,
        // pad_length: usize,
        // prefix: bool,
    },
}

impl Piece<'_> {
    fn is_str(&self) -> bool {
        matches!(self, Piece::Str(_))
    }
}

fn mk_name_with_idx(prefix: &str, i: usize) -> Ident {
    Ident::new(&format!("{}_{}", prefix, i), Span::call_site())
}

// `}}` -> `}`
fn unescape(mut literal: &str, span: Span) -> parse::Result<Cow<str>> {
    if literal.contains('}') {
        let mut buf = String::new();

        while literal.contains('}') {
            const ERR: &str = "format string contains an unmatched right brace";
            let mut parts = literal.splitn(2, '}');

            match (parts.next(), parts.next()) {
                (Some(left), Some(right)) => {
                    const ESCAPED_BRACE: &str = "}";

                    if let Some(tail) = right.strip_prefix(ESCAPED_BRACE) {
                        buf.push_str(left);
                        buf.push('}');

                        literal = tail;
                    } else {
                        return Err(parse::Error::new(span, ERR));
                    }
                }

                _ => unreachable!(),
            }
        }

        buf.push_str(literal);

        Ok(buf.into())
    } else {
        Ok(Cow::Borrowed(literal))
    }
}

fn parse_c_format(literal: &str, span: Span) -> parse::Result<Vec<Piece>> {
    let mut pieces = vec![];

    let chars = &mut literal.chars();
    while let Some(ch) = chars.next() {
        if ch != '%' {
            continue;
        }
        if let Some(lb) = chars.next() {
            match lb {
                'd' => {
                    pieces.push(Piece::Percent(PCT::Int));
                }
                'c' => {
                    pieces.push(Piece::Percent(PCT::Ch));
                }
                's' | 'S' => {
                    pieces.push(Piece::Percent(PCT::Str));
                }
                'x' | 'X' => {
                    pieces.push(Piece::Percent(PCT::Hex));
                }
                'y' | 'Y' => {
                    pieces.push(Piece::Percent(PCT::Array));
                }
                '%' => {}
                _ => {
                    return Err(parse::Error::new(
                        span,
                        "invalid format string: expected `%d`, `%c`, `%s/S`, `%x/X` or `%y/Y`",
                    ));
                }
            }
        }
    }
    if literal.contains("%s") {
        let literal = literal.replace("%s", "%S");
        pieces.push(Piece::Str(literal.into()));
    } else {
        pieces.push(Piece::Str(literal.into()));
    }

    Ok(pieces)
}
fn parse(mut literal: &str, span: Span) -> parse::Result<Vec<Piece>> {
    let mut pieces = vec![];

    let mut buf = String::new();
    loop {
        let mut parts = literal.splitn(2, '{');
        match (parts.next(), parts.next()) {
            // empty string literal
            (None, None) => break,

            // end of the string literal
            (Some(s), None) => {
                if pieces.is_empty() {
                    return parse_c_format(literal, span);
                } else if buf.is_empty() {
                    if !s.is_empty() {
                        pieces.push(Piece::Str(unescape(s, span)?));
                    }
                } else {
                    buf.push_str(&unescape(s, span)?);

                    pieces.push(Piece::Str(Cow::Owned(buf)));
                }

                break;
            }

            (head, Some(tail)) => {
                const DEBUG: &str = ":?}";
                const DEBUG_PRETTY: &str = ":#?}";
                const DISPLAY: &str = "}";
                const ESCAPED_BRACE: &str = "{";

                let head = head.unwrap_or("");
                if tail.starts_with(DEBUG)
                    || tail.starts_with(DEBUG_PRETTY)
                    || tail.starts_with(DISPLAY)
                    || tail.starts_with(':')
                {
                    if buf.is_empty() {
                        if !head.is_empty() {
                            pieces.push(Piece::Str(unescape(head, span)?));
                        }
                    } else {
                        buf.push_str(&unescape(head, span)?);

                        pieces.push(Piece::Str(Cow::Owned(mem::take(&mut buf))));
                    }

                    if let Some(tail_tail) = tail.strip_prefix(DEBUG) {
                        pieces.push(Piece::Debug);

                        literal = tail_tail;
                    } else if let Some(tail_tail) = tail.strip_prefix(DEBUG_PRETTY) {
                        pieces.push(Piece::Debug);

                        literal = tail_tail;
                    } else if let Some(tail2) = tail.strip_prefix(':') {
                        let (piece, remainder) = parse_colon(tail2, span)?;
                        pieces.push(piece);
                        literal = remainder;
                    } else {
                        pieces.push(Piece::Display);

                        literal = &tail[DISPLAY.len()..];
                    }
                } else if let Some(tail_tail) = tail.strip_prefix(ESCAPED_BRACE) {
                    buf.push_str(&unescape(head, span)?);
                    buf.push('{');

                    literal = tail_tail;
                } else {
                    return Err(parse::Error::new(
                        span,
                        "invalid format string: expected `{{`, `{}`, `{:?}` or `{:#?}`",
                    ));
                }
            }
        }
    }

    Ok(pieces)
}

/// given a string src that begins with a text decimal number, return the tail (characters after the number) and the value of the decimal number
fn split_number(src: &str) -> (&str, usize) {
    let mut rval = 0;
    let mut cursor = 0;

    let chars = src.chars();
    for (i, ch) in chars.enumerate() {
        match ch.to_digit(10) {
            Some(val) => {
                rval = rval * 10 + val as usize;
                cursor = i + 1;
            }
            None => break,
        }
    }

    (&src[cursor..], rval)
}

/// parses the stuff after a `{:` into a [Piece] and the trailing `&str` (what comes after the `}`)
fn parse_colon(format: &str, span: Span) -> parse::Result<(Piece, &str)> {
    let (format, _prefix) = if let Some(tail) = format.strip_prefix('#') {
        (tail, true)
    } else {
        (format, false)
    };
    let (format, _pad_char) = if let Some(tail) = format.strip_prefix('0') {
        (tail, b'0')
    } else {
        (format, b' ')
    };
    let (format, _pad_length) = if !format.is_empty()
        && if let Some(ch) = format.chars().next() {
            ch.is_ascii_digit()
        } else {
            false
        } {
        split_number(format)
    } else {
        (format, 0)
    };
    if let Some(tail) = format.strip_prefix("x}") {
        Ok((
            Piece::Hex {
                upper_case: false,
                // pad_char,
                // pad_length,
                // prefix,
            },
            tail,
        ))
    } else if let Some(tail) = format.strip_prefix("X}") {
        Ok((
            Piece::Hex {
                upper_case: true,
                // pad_char,
                // pad_length,
                // prefix,
            },
            tail,
        ))
    } else {
        Err(parse::Error::new(
            span,
            "invalid format string: expected `{{`, `{}`, `{:?}`, `{:#?}` or '{:x}'",
        ))
    }
}

fn map_expr_error(expr: &Expr) -> String {
    let expr_type = match expr {
        Expr::Assign(_) => "Assign",
        Expr::Async(_) => "Async",
        Expr::Await(_) => "Await",
        Expr::Binary(_) => "Binary",
        Expr::Block(_) => "Block",
        Expr::Break(_) => "Break",
        Expr::Call(_) => "Call",
        Expr::Cast(_) => "Cast",
        Expr::Closure(_) => "Closure",
        Expr::Const(_) => "Const",
        Expr::Continue(_) => "Continue",
        Expr::Field(_) => "Field",
        Expr::ForLoop(_) => "ForLoop",
        Expr::Group(_) => "Group",
        Expr::If(_) => "If",
        Expr::Index(_) => "Index",
        Expr::Infer(_) => "Infer",
        Expr::Let(_) => "Let",
        Expr::Loop(_) => "Loop",
        Expr::Macro(_) => "Macro",
        Expr::Match(_) => "Match",
        Expr::MethodCall(_) => "MethodCall",
        Expr::Paren(_) => "Paren",
        Expr::Range(_) => "Range",
        Expr::Repeat(_) => "Repeat",
        Expr::Return(_) => "Return",
        Expr::Struct(_) => "Struct",
        Expr::Try(_) => "Try",
        Expr::TryBlock(_) => "TryBlock",
        Expr::Tuple(_) => "Tuple",
        Expr::Unary(_) => "Unary",
        Expr::Unsafe(_) => "Unsafe",
        Expr::Verbatim(_) => "Verbatim",
        Expr::While(_) => "While",
        Expr::Yield(_) => "Yield",
        Expr::Array(_) => "Array",
        Expr::Lit(_) => "Lit",
        Expr::Path(_) => "Path",
        Expr::Reference(_) => "Reference",
        _ => "Unknow",
    };

    let mut msg = "Expr not implement: ".to_string();
    msg.push_str(expr_type);

    return msg;
}
