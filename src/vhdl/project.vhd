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
    
    constant bit_width : natural  := 8;
    constant ram_bytes : natural := 256;
    type aRam is array (0 to ram_bytes-1) of std_ulogic_vector(7 downto 0);

    -- Example control: use uio_in for address
    signal address : unsigned(7 downto 0);
    signal write_enable : std_ulogic;
    signal sRam : aRam := (others => (others => '0'));
    
begin
    -- Example mapping (adjust based on your needs):
    -- ui_in = data to write
    -- uio_in = address
    -- uio_in(7) could be write enable (just an example)
    
    address <= unsigned(uio_in(6 downto 0)) & '0'; -- 7-bit address
    write_enable <= uio_in(7); -- MSB as write enable
    
    process (clk, rst_n)
    begin
        if rst_n = '0' then
            sRam <= (others => (others => '0'));
            uo_out <= (others => '0');
        elsif rising_edge(clk) then
            if write_enable = '1' then
                sRam(to_integer(address)) <= ui_in;
            end if;
            -- Read operation
            uo_out <= sRam(to_integer(address));
        end if;
    end process;
    
    -- Set unused outputs
    uio_out <= (others => '0');
    uio_oe <= (others => '0'); -- All as inputs

end architecture rtl;