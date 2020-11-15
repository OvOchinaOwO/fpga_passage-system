
module dvi_transmitter_top(
    input        pclk,           // pixel clock
    input        pclk_x5,        // pixel clock x5
    input        reset_n,        // reset
    
    input [23:0] video_din,      // RGB888 video in
    input        video_hsync,    // hsync data
    input        video_vsync,    // vsync data
    input        video_de,       // data enable
    
    output       tmds_clk_p,    // TMDS 时钟通道
    output       tmds_clk_n,
    output [2:0] tmds_data_p,   // TMDS 数据通道
    output [2:0] tmds_data_n,
    output       tmds_oen       // TMDS 输出使能
    );
    
//wire define    
wire        reset;
    
//并行数据
wire [9:0]  red_10bit;
wire [9:0]  green_10bit;
wire [9:0]  blue_10bit;
wire [9:0]  clk_10bit;  
  
//串行数据
wire [2:0]  tmds_data_serial;
wire        tmds_clk_serial;

//*****************************************************
//**                    main code
//***************************************************** 
assign tmds_oen = 1'b1;  
assign clk_10bit = 10'b1111100000;

//异步复位，同步释放
asyn_rst_syn reset_syn(
    .reset_n    (reset_n),
    .clk        (pclk),
    
    .syn_reset  (reset)    //高有效
    );
  
//对三个颜色通道进行编码
dvi_encoder encoder_b (
    .clkin      (pclk),
    .rstin	    (reset),
    
    .din        (video_din[7:0]),
    .c0			(video_hsync),
    .c1			(video_vsync),
    .de			(video_de),
    .dout		(blue_10bit)
    ) ;

dvi_encoder encoder_g (
    .clkin      (pclk),
    .rstin	    (reset),
    
    .din		(video_din[15:8]),
    .c0			(1'b0),
    .c1			(1'b0),
    .de			(video_de),
    .dout		(green_10bit)
    ) ;
    
dvi_encoder encoder_r (
    .clkin      (pclk),
    .rstin	    (reset),
    
    .din		(video_din[23:16]),
    .c0			(1'b0),
    .c1			(1'b0),
    .de			(video_de),
    .dout		(red_10bit)
    ) ;
    

    

  
endmodule