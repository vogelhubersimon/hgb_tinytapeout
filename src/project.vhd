-- verilog mapping
    -- input  wire [7:0] ui_in,    // Dedicated inputs
    -- output wire [7:0] uo_out,   // Dedicated outputs
    -- input  wire [7:0] uio_in,   // IOs: Input path
    -- output wire [7:0] uio_out,  // IOs: Output path
    -- output wire [7:0] uio_oe,   // IOs: Enable path (active high: 0=input, 1=output)
    -- input  wire       ena,      // always 1 when the design is powered, so you can ignore it
    -- input  wire       clk,      // clock
    -- input  wire       rst_n     // reset_n - low to rese

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity tt_um_example is
    port (
        ui_in  : in  std_ulogic_vector(7 downto 0); -- Dedicated inputs
        uo_out : out std_ulogic_vector(7 downto 0); -- Dedicated outputs
        uio_in : in  std_ulogic_vector(7 downto 0); -- IOs: Input path
        uio_out: out std_ulogic_vector(7 downto 0); -- IOs: Output path
        uio_oe : out std_ulogic_vector(7 downto 0); -- IOs: Enable path (active high: 0=input, 1=output)
        ena    : in  std_ulogic; -- always 1 when the design is powered, so you can ignore it
        clk    : in  std_ulogic; -- clock
        rst_n  : in  std_ulogic -- reset_n - low to reset
    );
end entity tt_um_example;

-- shift register
architecture rtl of tt_um_example is
    signal reg : std_ulogic_vector(7 downto 0);
begin
    
    process (clk, rst_n)
    begin
        if rst_n = '0' then
            reg <= (others => '0');
        elsif rising_edge(clk) then
            reg(7) <= ui_in(0);
            reg(6 downto 0) <= reg(7 downto 1);
        end if;
    end process;

    uo_out <= reg;

end architecture rtl;