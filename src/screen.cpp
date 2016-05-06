/**
 * Licencia GPLv3
 *
 * Este archivo es parte de libWiiEsp. Copyright (C) 2011 Ezequiel Vázquez de la Calle
 *
 * libWiiEsp es software libre: usted puede redistribuirlo y/o modificarlo bajo los términos de la 
 * Licencia Pública General GNU publicada por la Fundación para el Software Libre, ya sea la versión 3 
 * de la Licencia, o (a su elección) cualquier versión posterior.
 *
 * libWiiEsp se distribuye con la esperanza de que sea útil, pero SIN GARANTÍA ALGUNA; ni siquiera 
 * la garantía implícita MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. Consulte los detalles de
 * la Licencia Pública General GNU para obtener una información más detallada.
 *
 * Debería haber recibido una copia de la Licencia Pública General GNU junto a libWiiEsp. En caso 
 * contrario, consulte <http://www.gnu.org/licenses/>.
 *
 */

#include "screen.h"
using namespace std;

Screen* Screen::_instance = 0;

void Screen::inicializar(void)
{
	_fifoBuffer = NULL;
	_frame_actual = 0;
	_update_gfx = 0;
	_update_scr = 0;
	_backgroundColor = {0, 0x20*0, 0x40*0, 255};

	// Inicialización básica del sistema de vídeo de la consola.
	VIDEO_Init();

	// Obtener el modo de pantalla que tenga la consola.
	_screenMode = VIDEO_GetPreferredMode(NULL);
	// Inicializar el sistema de doble buffer a partir del modo de pantalla que tenga la consola.
	_frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(_screenMode));
	_frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(_screenMode));

	// Aplicar el modo de pantalla al chip gráfico
	VIDEO_Configure(_screenMode);
	// Establecer el siguiente frameBuffer
	VIDEO_SetNextFramebuffer(_frameBuffer[_frame_actual]);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();

	// Si el modo de pantalla no tiene entrelazado, hay que sincronizar ahora
	if(_screenMode->viTVMode & VI_NON_INTERLACE)
		VIDEO_WaitVSync();

	// Asigna la memoria para el FIFO, el "bus" que envía los datos al procesador gráfico en cada frame.
	_fifoBuffer = MEM_K0_TO_K1(memalign(32, FIFO_SIZE));
	memset(_fifoBuffer, 0, FIFO_SIZE);

	// Inicializar el procesador gráfico (y la librería gráfica de bajo nivel GX)
	GX_Init(_fifoBuffer, FIFO_SIZE);
	// Establecer un formato RGB para el color, con un buffer de 24 bits
	GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
	// Asignar los valores de borrado de pantalla (color) y de relleno de buffer
	GX_SetCopyClear(_backgroundColor, 0x00FFFFFF);

	// Establecer el ancho y el alto de la pantalla
	_ancho_pantalla = _screenMode->fbWidth;
	_alto_pantalla = _screenMode->efbHeight;

	// Establecer el viewport: el rectángulo donde se proyectará lo que haya en el buffer de gráficos
	GX_SetViewport(0, 0, _screenMode->fbWidth, _screenMode->efbHeight, 0.0f, 1.0f);

	// Inicializar los detalles de las GX
	GX_SetDispCopyYScale((f32)_screenMode->xfbHeight/(f32)_screenMode->efbHeight);
	GX_SetScissor(0, 0, _screenMode->fbWidth, _screenMode->efbHeight);
	GX_SetDispCopySrc(0, 0, _screenMode->fbWidth, _screenMode->efbHeight);
	GX_SetDispCopyDst(_screenMode->fbWidth, _screenMode->xfbHeight);
	GX_SetCopyFilter(_screenMode->aa, _screenMode->sample_pattern, GX_TRUE, _screenMode->vfilter);
	GX_SetFieldMode(_screenMode->field_rendering,
					((_screenMode->viHeight == 2 * _screenMode->xfbHeight) ? GX_ENABLE : GX_DISABLE));
	GX_SetCullMode(GX_CULL_NONE);
	GX_SetZMode(GX_TRUE, GX_LEQUAL,	GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(_frameBuffer[_frame_actual], GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);

	// Ajusta la Matriz de Proyeccion para trabajar con sprites en 2D
    if(_screenMode->efbHeight <= 480)
		guOrtho(_proyeccion,
				-12.0f,
				(f32)(_screenMode->efbHeight + 16),
				0.0f,
				(f32)(_screenMode->fbWidth - 1),
				0.0f,
				1000.0f);
	else
		guOrtho(_proyeccion,
				0.0f,
				(f32)(_screenMode->efbHeight),
				0.0f,
				(f32)(_screenMode->fbWidth - 1),
				0.0f,
				1000.0f);

	// Cargar la matriz de proyección
	GX_LoadProjectionMtx(_proyeccion, GX_ORTHOGRAPHIC);
	// Función de sincronización
	GX_PixModeSync();
	// El buffer de texturas (FIFO) se actualizará después de aplicar las texturas, no antes
	GX_SetZCompLoc(GX_FALSE);

    // Tratamiento del color de los píxeles
	GX_SetNumChans(1); 
	GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_CLAMP, GX_AF_NONE);
	GX_SetNumTevStages(1);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);

	// Ajustar a una textura por operación
	GX_SetNumTexGens(1);

	// Invalidar todas las texturas, esto se hace para preparar las GX y que puedan trabajar inmediatamente con texturas
	GX_InvVtxCache();
	GX_InvalidateTexAll();

	// Establecer la matriz mundial
	guMtxIdentity(_modelView);
	// Carga la matriz mundial como identidad
	GX_LoadPosMtxImm(_modelView, GX_PNMTX0);
	// Deja seleccionada la matriz mundial
	GX_SetCurrentMtx(GX_PNMTX0);
    
	// Limpiar la pantalla actualizándola dos veces
	_update_gfx = 1;
	flip();
	_update_gfx = 1;
	flip();
}

u16 Screen::ancho(void) const
{
	return _ancho_pantalla;
}

u16 Screen::alto(void) const
{
	return _alto_pantalla;
}

void Screen::flip(void)
{
	// Si se ha marcado que hay que actualizar los gráficos, dar por finalizado el frame actual
	if(_update_gfx)
	{
		GX_DrawDone();
		_update_scr = 1;
	}
	else
		// Si no se ha marcado la actualización de gráficos, no hay que actualizar la pantalla
		_update_scr = 0;

	// Esperar la sincronización vertical
	VIDEO_WaitVSync();
	// Si hay que actualizar los gráficos y la pantalla
	if(_update_gfx and _update_scr)
	{
		// Marcar los gráficos como actualizados
		_update_gfx = 0;
		// Copiar el frame buffer actual en la pantalla
		GX_CopyDisp(_frameBuffer[_frame_actual], GX_TRUE);
		// Fijar los datos
		GX_Flush();
		// Establecer el nuevo "siguiente frame buffer"
		VIDEO_SetNextFramebuffer(_frameBuffer[_frame_actual]);
		// Calcular el nuevo "actual frame buffer"
		_frame_actual ^= 1;
		// Fijar los datos en el sistema de vídeo
		VIDEO_Flush();
		// Marcar la pantalla como actualizada
		_update_scr = 0;
	}

	// Limpiar la caché de vértices y texturas para dejarla lista para el siguiente frame
	GX_InvVtxCache();
	GX_InvalidateTexAll();
}

// Operaciones con texturas

void Screen::crearTextura(GXTexObj* textura, void* pixeles, u16 ancho, u16 alto)
{
	// Organizar los pixeles en tiles de 4x4 para formato TILE_RGB5A3
	tiling4x4(pixeles, ancho, alto);
	// Preparar la creación de la textura
	GX_TexModeSync();
	// Inicializar el objeto de textura GXTexObj
	GX_InitTexObj(textura, pixeles, ancho, alto, GX_TF_RGB5A3, GX_CLAMP, GX_CLAMP, GX_FALSE);
	// Aplicar filtros GX_LINEAR
	GX_InitTexObjLOD(textura, GX_LINEAR, GX_LINEAR, 0, 0, 0, 0, 0, GX_ANISO_1);
}

void Screen::dibujarTextura(GXTexObj *textura, s16 x, s16 y, s16 z, u16 ancho, u16 alto)
{
	// Si la coordenada Z es negativa (por error del usuario), se le cambia el signo
	if(z < 0)
		z = -z;

	// Preparar el procesador gráfico para dibujar una textura
	configurarTextura(textura, 0);

	// Dibujar un cuadrado relleno con la textura (con escalado 1:1, es decir, tal cual)
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);

	// Primer vértice (superior izquierdo)
	GX_Position3s16(x, y, -z);
	GX_Color1u32(0xFFFFFFFF);
	GX_TexCoord2s16(0, 0);

	// Segundo vértice (superior derecho)
	GX_Position3s16(x + ancho, y, -z);
	GX_Color1u32(0xFFFFFFFF);
	GX_TexCoord2s16(1, 0);

	// Tercer vértice (inferior derecho)
	GX_Position3s16(x + ancho, y + alto, -z);
	GX_Color1u32(0xFFFFFFFF);
	GX_TexCoord2s16(1, 1);

	// Cuarto vértice (inferior izquierdo)
	GX_Position3s16(x, y + alto, -z);
	GX_Color1u32(0xFFFFFFFF);
	GX_TexCoord2s16(0, 1);

	GX_End();
}

void Screen::dibujarCuadro(GXTexObj* textura, u16 texAncho, u16 texAlto, s16 x, s16 y, s16 z,
							s16 cuadroX, s16 cuadroY, u16 cuadroAncho, u16 cuadroAlto, bool invertido)
{
	// Si la coordenada Z es negativa (por error del usuario), se le cambia el signo
	if(z < 0)
		z = -z;

	// Preparar el procesador gráfico para dibujar una textura
	configurarTextura(textura, 10);

	// Calcular el ancho y el alto del cuadro de la textura que se quiere dibujar, aplicando una escala de 10
	// Si se quiere dibujar el cuadro invertido, se desplaza el primer punto (izquierda arriba) horizontalmente
	// para que coincida con el segundo, y se pone en negativo el ancho. Así se engaña al sistema de vídeo, que
	// recorrerá el cuadro origen en el orden (der-arr, izq-arr, izq-aba, der-aba) la textura, y en el orden
	// habitual el cuadro destino, dibujando la textura invertida sobre el eje vertical.
	s16 w = (1023 * cuadroAncho) / texAncho * (invertido ? -1 : 1);
	s16 h = (1023 * cuadroAlto) / texAlto;
	s16 tx = (1023 * (cuadroX + (invertido ? cuadroAncho : 0))) / texAncho;
	s16 ty = (1023 * cuadroY) / texAlto;

	// Dibujando la parte de la textura que se quiere dibujar
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		 
	GX_Position3s16(x, y, -z); 
	GX_Color1u32(0xFFFFFFFF);
	GX_TexCoord2s16(tx, ty);

	GX_Position3s16(x + cuadroAncho, y, -z);
	GX_Color1u32(0xFFFFFFFF);
	GX_TexCoord2s16(tx + w, ty);

	GX_Position3s16(x + cuadroAncho, y + cuadroAlto, -z);
	GX_Color1u32(0xFFFFFFFF);
	GX_TexCoord2s16(tx + w, ty + h);

	GX_Position3s16(x, y + cuadroAlto, -z);
	GX_Color1u32(0xFFFFFFFF);
	GX_TexCoord2s16(tx, ty + h);

	GX_End();
}

// Dibujo de formas geométricas

void Screen::dibujarPunto(s16 x, s16 y, s16 z, u32 color)
{
	// Si la coordenada Z es negativa (por error del usuario), se le cambia el signo
	if(z < 0)
		z = -z;

	// Preparar el procesador gráfico para dibujar color directo
	configurarColor();

	// Dibujar un punto de color
	GX_Begin(GX_POINTS, GX_VTXFMT0, 1);	
	// Establecer la posición con las coordenadas
	GX_Position3s16(x, y, -z);
	// Pintar con el color deseado el punto con las coordenadas indicadas en la instrucción anterior
	GX_Color1u32(color);
	// Fin del dibujo de un punto de color
	GX_End();
}

void Screen::dibujarLinea(s16 x1, s16 y1, s16 x2, s16 y2, s16 z, u16 ancho, u32 color)
{
	// Si la coordenada Z es negativa (por error del usuario), se le cambia el signo
	if(z < 0)
		z = -z;

	// Preparar el procesador gráfico para dibujar color directo
	configurarColor();

	// Como la línea podrá tener más de un píxel de ancho, realmente se va a dibujar un rectángulo
	s16 x3, y3, x4, y4;

	// Se espera que el punto 1 esté más a la izquierda que el punto 2, en caso contrario, se intercambian los puntos
	// Se utiliza como variable auxiliar la coordenada X del punto 3 (auxiliar)
	if (x2 < x1)
	{
		x3 = x2;
		x2 = x1;
		x1 = x3;
		x3 = y2;
		y2 = y1;
		y1 = x3;
	}

	// Diferencia entre x1 y x2, en absoluto, y sumando 1
	s16 diff_x = ((x2 - x1 < 0) ? x1 - x2 : x2 - x1) + 1;
	// Diferencia entre y1 y y2, en absoluto, y sumando 1
	s16 diff_y = ((y2 - y1 < 0) ? y1 - y2 : y2 - y1) + 1;

	// Realizar los cálculos necesarios para dotar de ancho a la línea (calcular los puntos 3 y 4) ...
	// ... si las coordenadas Y de los puntos 1 y 2 coinciden,
	// o si la diferencia entre las coordenadas X es mayor que la diferencia entre las coordenadas Y
	if((y1 == y2 and x1 != x2) or (diff_x > diff_y))
	{
		x3 = x1;
		y3 = y1 + ancho;
		x4 = x2;
		y4 = y2 + ancho;
	}
	// ... si las coordenadas X de los puntos 1 y 2 coinciden,
	// o si la diferencia entre las coordenadas Y es mayor que la diferencia entre las coordenadas X
	else if((y1 != y2 and x1 == x2) or (diff_x <= diff_y))
	{
		x3 = x1 + ancho;
		y3 = y1;
		x4 = x2 + ancho;
		y4 = y2;
	}

	// Dibujar los vértices, utilizando dos triángulos con un lado común para dibujar el rectángulo
	// que compondrá la línea con ancho
	GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 6);

	// Primer triángulo
	GX_Position3s16(x1, y1, -z);
	GX_Color1u32(color);

	GX_Position3s16(x2, y2, -z);
	GX_Color1u32(color);

	GX_Position3s16(x3, y3, -z);
	GX_Color1u32(color);

	// Segundo triángulo: comparten los vértices 2 y 3
	GX_Position3s16(x2, y2, -z);
	GX_Color1u32(color);

	GX_Position3s16(x3, y3, -z);
	GX_Color1u32(color);
	
	GX_Position3s16(x4, y4, -z);
	GX_Color1u32(color);

	GX_End();
}

void Screen::dibujarRectangulo(s16 x1, s16 y1, s16 x2, s16 y2, s16 x3, s16 y3, s16 x4, s16 y4, s16 z, u32 color)
{
	// Si la coordenada Z es negativa (por error del usuario), se le cambia el signo
	if(z < 0)
		z = -z;

	// Preparar el procesador gráfico para dibujar color directo
	configurarColor();

	// Dibujar un rectangulo (4 vertices) relleno de color
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);

	GX_Position3s16(x1, y1, -z); 
	GX_Color1u32(color);

	GX_Position3s16(x2, y2, -z); 
	GX_Color1u32(color);

	GX_Position3s16(x3, y3, -z); 
	GX_Color1u32(color);

	GX_Position3s16(x4, y4, -z); 
	GX_Color1u32(color);

	GX_End();
}

void Screen::dibujarCirculo(s16 x, s16 y, s16 z, f32 r, u32 color)
{
	// Si la coordenada Z es negativa (por error del usuario), se le cambia el signo
	if(z < 0)
		z = -z;

	// Para dibujar el tamaño correcto
	r = r/2;

	// Preparar el procesador gráfico para dibujar color directo
	configurarColor();

	for(u32 n = 0 ; n < 16384 ; n += 512)
	{
		u32 na = 512;
		if((n + 512) > 16384)
			na = 16384 - n;

		GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);

		GX_Position3s16(x + (s16)(r * seno(n)/16384), y - (s16)(r * coseno(n)/16384), -z); 
		GX_Color1u32(color);

		GX_Position3s16(x + (s16)(r * seno(n + na)/16384), y - (s16)(r * coseno(n+na)/16384), -z); 
		GX_Color1u32(color);

		GX_Position3s16(x, y, -z); 
		GX_Color1u32(color);

		GX_End();

	}
}

// Metodos protegidos

Screen::~Screen(void)
{
	flip();
}

// Métodos privados

void Screen::tiling4x4(void* mem, u16 ancho, u16 alto)
{
	u16 mem_tile[1024*8];
	u16* p1 = (u16*) mem;
    u16* p2 = (u16*) &mem_tile[0];

    for(u16 n = 0 ; n < alto ; n += 4)
	{
		for(u16 l = 0 ; l < 4 ; l++)
		{
			for(u16 m = 0 ; m < ancho ; m += 4)
			{
				p2[((l+m)<<2)] = p1[(n+l)*ancho+m];
				p2[((l+m)<<2)+1] = p1[(n+l)*ancho+m+1];
				p2[((l+m)<<2)+2] = p1[(n+l)*ancho+m+2];
				p2[((l+m)<<2)+3] = p1[(n+l)*ancho+m+3];
			}
		}
		for(u16 l = 0 ; l < 4 ; l++)
		{
			for(u16 m = 0 ; m < ancho ; m++)
				p1[(n+l)*ancho+m] = p2[(l)*ancho+m];
		}
	}
	DCFlushRange(mem, ancho * alto * 2);
}

u32 Screen::seno(u32 ang)
{
	f32 PID = 6.283185307179586476925286766559;
	u32 n = ang;

	if(n < 0)
		n = 16384 - n;
	n &= 16383;

	return (u32)((f32) 16384 * sin((PID * (f32)n)/16384.0));
}

u32 Screen::coseno(u32 ang)
{
	f32 PID = 6.283185307179586476925286766559;
	u32 n = ang;

	if(n < 0)
		n = 16384 - n;
	n &= 16383;

	return (u32)((f32) 16384 * cos((PID * (f32)n)/16384.0));
}

void Screen::configurarColor(void)
{
	// Preparar las GX para dibujar un color (textura nula)
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetAlphaCompare(GX_GEQUAL, 8, GX_AOP_AND, GX_ALWAYS, 0);
	GX_SetNumTevStages(1);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	GX_SetVtxDesc(GX_VA_TEX0, GX_NONE);

	// Descriptores de vértices
	GX_ClearVtxDesc();													// borra los descriptores
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);								// selecciona Posicion como directo
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);								// selecciona Color como directo 
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);		// formato de posicion S16 para X, Y ,Z
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);	// color RGBA8 para color0

	// Marcar el flag de actualización de gráficos
	_update_gfx = 1;
}

void Screen::configurarTextura(GXTexObj* textura, u8 escala)
{
	// Preparar las GX para dibujar una textura
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA,GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetAlphaCompare(GX_GEQUAL, 8, GX_AOP_AND, GX_ALWAYS, 0);
    GX_LoadTexObj(textura, GX_TEXMAP0);
	GX_SetNumTevStages(1);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GX_SetNumTexGens(1);

	// Descriptores de vértices
	GX_ClearVtxDesc();						// borra los descriptores
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);	// selecciona Posicion como directo
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);	// selecciona Color como directo  
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);	// selecciona Textura como directa

	// Formato de los parámetros de posición, color y posición en textura
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, escala);

	// Marcar el flag de actualización de gráficos
	_update_gfx = 1;
}

