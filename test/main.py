# import pandas as pd

# FILE_PATH = "/Users/adityashigam/Documents/pixiq/pixiqCodes/test/CHINA EXTRA PART.xlsx"

# # # ---------------------------------------------------------------------------
# # # Sheet configurations: (sheet_name, value_col_idx, package_col_idx, qty_col_idx)
# # # col indices are positional (0-based) after reading with header=None
# # # ---------------------------------------------------------------------------
# # SHEET_CONFIG = {
# #     "RESISTOR": None,        # special multi-table layout, handled separately
# #     "CAPACITOR": None,       # special multi-table layout, handled separately
# #     "MCU":              {"value": 0, "package": None, "qty": 1},
# #     "Diodes":           {"value": 0, "package": 1,    "qty": 2},
# #     "GATE DRIVER  MIC": {"value": 0, "package": 1,    "qty": 2},
# #     "BUCK LDO":         {"value": 0, "package": 1,    "qty": 2},
# #     "MOSFET":           {"value": 0, "package": 1,    "qty": 2},
# #     "IC  BUFFER":       {"value": 0, "package": 1,    "qty": 2},
# #     "INDUCTOR":         {"value": 0, "package": 1,    "qty": 2},
# #     "CPOL CAPACITOR":   {"value": 0, "package": 1,    "qty": 2},
# #     "CONNECTOR":        {"value": 0, "package": 1,    "qty": 2},
# #     "OSCILLATOR":       {"value": 0, "package": 1,    "qty": 2},
# # }


# # def parse_multi_table_sheet(raw_df, tables_per_row=4):
# #     """
# #     Parse sheets that have multiple side-by-side tables (RESISTOR, CAPACITOR).
# #     Each table block is 3 columns wide: Value | Package | Quantity.
# #     Returns a unified DataFrame with columns: Value, Package, Quantity.
# #     """
# #     records = []
# #     num_cols = raw_df.shape[1]
# #     cols_per_table = 3

# #     for start_col in range(0, num_cols, cols_per_table):
# #         block = raw_df.iloc[:, start_col: start_col + cols_per_table]
# #         if block.shape[1] < 3:
# #             continue

# #         # Find the header row inside the block (row where col-0 == 'Value')
# #         header_row = None
# #         for i, cell in enumerate(block.iloc[:, 0]):
# #             if str(cell).strip().lower() == "value":
# #                 header_row = i
# #                 break

# #         if header_row is None:
# #             continue

# #         data = block.iloc[header_row + 1:].copy()
# #         data.columns = ["Value", "Package", "Quantity"]
# #         data = data.dropna(subset=["Value"])
# #         data = data[data["Value"].astype(str).str.strip() != ""]
# #         # Skip rows that look like another header
# #         data = data[data["Value"].astype(str).str.strip().str.lower() != "value"]
# #         records.append(data)

# #     if not records:
# #         return pd.DataFrame(columns=["Value", "Package", "Quantity"])
# #     return pd.concat(records, ignore_index=True)


# # def parse_standard_sheet(raw_df, cfg):
# #     """
# #     Parse a standard sheet using column index config.
# #     Skips the first row which is a header label, then treats row-1 as column names.
# #     """
# #     # Find the header row (where the value col contains 'Value', 'Model', 'iC Name', etc.)
# #     header_markers = {"value", "model", "ic name", "capacitor", "diodes", "mosfet"}
# #     header_row = 0
# #     for i, cell in enumerate(raw_df.iloc[:, cfg["value"]]):
# #         if str(cell).strip().lower() in header_markers:
# #             header_row = i
# #             break

# #     data = raw_df.iloc[header_row + 1:].copy()
# #     data = data.dropna(subset=[raw_df.columns[cfg["value"]]])

# #     result = pd.DataFrame()
# #     result["Value"]    = data.iloc[:, cfg["value"]].astype(str).str.strip()
# #     result["Package"]  = data.iloc[:, cfg["package"]].astype(str).str.strip() if cfg["package"] is not None else "N/A"
# #     result["Quantity"] = data.iloc[:, cfg["qty"]].astype(str).str.strip()

# #     return result[result["Value"] != ""]


# # def search_part(part_name: str, file_path: str = FILE_PATH):
# #     """
# #     Search for a part across all sheets in the Excel file.
# #     Matching is case-insensitive and partial (substring).

# #     Returns a list of dicts with keys: Sheet, Value, Package, Quantity
# #     """
# #     part_lower = part_name.strip().lower()
# #     all_sheets_raw = pd.read_excel(file_path, sheet_name=None, header=None)
# #     results = []

# #     for sheet_name, raw_df in all_sheets_raw.items():
# #         raw_df = raw_df.fillna("")

# #         if sheet_name in ("RESISTOR", "CAPACITOR"):
# #             df = parse_multi_table_sheet(raw_df)
# #         elif sheet_name in SHEET_CONFIG:
# #             cfg = SHEET_CONFIG[sheet_name]
# #             if cfg is None:
# #                 continue
# #             df = parse_standard_sheet(raw_df, cfg)
# #         else:
# #             continue

# #         # Search
# #         mask = df["Value"].str.lower().str.contains(part_lower, na=False)
# #         matches = df[mask]

# #         for _, row in matches.iterrows():
# #             results.append({
# #                 "Sheet":    sheet_name,
# #                 "Value":    row["Value"],
# #                 "Package":  row.get("Package", "N/A"),
# #                 "Quantity": row.get("Quantity", "N/A"),
# #             })

# #     return results


# # # ---------------------------------------------------------------------------
# # # Main
# # # ---------------------------------------------------------------------------
# # if __name__ == "__main__":
# #     part_to_find = input("Enter part name / value to search: ").strip()

# #     if not part_to_find:
# #         print("No input provided.")
# #     else:
# #         matches = search_part(part_to_find)

# #         if not matches:
# #             print(f"\n❌  Part '{part_to_find}' not found in any sheet.")
# #         else:
# #             print(f"\n✅  Found {len(matches)} match(es) for '{part_to_find}':\n")
# #             print(f"{'Sheet':<25} {'Value':<25} {'Package':<30} {'Quantity'}")
# #             print("-" * 95)
# #             for m in matches:
# #                 print(f"{m['Sheet']:<25} {m['Value']:<25} {m['Package']:<30} {m['Quantity']}")

# # import pandas as pd

# # FILE_PATH = "1774337999718_CHINA_EXTRA_PART.xlsx"

# # ---------------------------------------------------------------------------
# # Sheet configurations: (sheet_name, value_col_idx, package_col_idx, qty_col_idx)
# # col indices are positional (0-based) after reading with header=None
# # ---------------------------------------------------------------------------
# SHEET_CONFIG = {
#     "RESISTOR": None,        # special multi-table layout, handled separately
#     "CAPACITOR": None,       # special multi-table layout, handled separately
#     "MCU":              {"value": 0, "package": None, "qty": 1},
#     "Diodes":           {"value": 0, "package": 1,    "qty": 2},
#     "GATE DRIVER  MIC": {"value": 0, "package": 1,    "qty": 2},
#     "BUCK LDO":         {"value": 0, "package": 1,    "qty": 2},
#     "MOSFET":           {"value": 0, "package": 1,    "qty": 2},
#     "IC  BUFFER":       {"value": 0, "package": 1,    "qty": 2},
#     "INDUCTOR":         {"value": 0, "package": 1,    "qty": 2},
#     "CPOL CAPACITOR":   {"value": 0, "package": 1,    "qty": 2},
#     "CONNECTOR":        {"value": 0, "package": 1,    "qty": 2},
#     "OSCILLATOR":       {"value": 0, "package": 1,    "qty": 2},
# }


# def parse_multi_table_sheet(raw_df, tables_per_row=4):
#     """
#     Parse sheets that have multiple side-by-side tables (RESISTOR, CAPACITOR).
#     Each table block is 3 columns wide: Value | Package | Quantity.
#     Returns a unified DataFrame with columns: Value, Package, Quantity.
#     """
#     records = []
#     num_cols = raw_df.shape[1]
#     cols_per_table = 3

#     for start_col in range(0, num_cols, cols_per_table):
#         block = raw_df.iloc[:, start_col: start_col + cols_per_table]
#         if block.shape[1] < 3:
#             continue

#         # Find the header row inside the block (row where col-0 == 'Value')
#         header_row = None
#         for i, cell in enumerate(block.iloc[:, 0]):
#             if str(cell).strip().lower() == "value":
#                 header_row = i
#                 break

#         if header_row is None:
#             continue

#         data = block.iloc[header_row + 1:].copy()
#         data.columns = ["Value", "Package", "Quantity"]
#         data = data.dropna(subset=["Value"])
#         data = data[data["Value"].astype(str).str.strip() != ""]
#         # Skip rows that look like another header
#         data = data[data["Value"].astype(str).str.strip().str.lower() != "value"]
#         records.append(data)

#     if not records:
#         return pd.DataFrame(columns=["Value", "Package", "Quantity"])
#     return pd.concat(records, ignore_index=True)


# def parse_standard_sheet(raw_df, cfg):
#     """
#     Parse a standard sheet using column index config.
#     Skips the first row which is a header label, then treats row-1 as column names.
#     """
#     # Find the header row (where the value col contains 'Value', 'Model', 'iC Name', etc.)
#     header_markers = {"value", "model", "ic name", "capacitor", "diodes", "mosfet"}
#     header_row = 0
#     for i, cell in enumerate(raw_df.iloc[:, cfg["value"]]):
#         if str(cell).strip().lower() in header_markers:
#             header_row = i
#             break

#     data = raw_df.iloc[header_row + 1:].copy()
#     data = data.dropna(subset=[raw_df.columns[cfg["value"]]])

#     result = pd.DataFrame()
#     result["Value"]    = data.iloc[:, cfg["value"]].astype(str).str.strip()
#     result["Package"]  = data.iloc[:, cfg["package"]].astype(str).str.strip() if cfg["package"] is not None else "N/A"
#     result["Quantity"] = data.iloc[:, cfg["qty"]].astype(str).str.strip()

#     return result[result["Value"] != ""]


# def search_part(part_name: str, file_path: str = FILE_PATH):
#     """
#     Search for a part across all sheets in the Excel file.
#     Matching is case-insensitive and partial (substring).

#     Returns a list of dicts with keys: Sheet, Value, Package, Quantity
#     """
#     part_lower = part_name.strip().lower()
#     all_sheets_raw = pd.read_excel(file_path, sheet_name=None, header=None)
#     results = []

#     for sheet_name, raw_df in all_sheets_raw.items():
#         raw_df = raw_df.fillna("")

#         if sheet_name in ("RESISTOR", "CAPACITOR"):
#             df = parse_multi_table_sheet(raw_df)
#         elif sheet_name in SHEET_CONFIG:
#             cfg = SHEET_CONFIG[sheet_name]
#             if cfg is None:
#                 continue
#             df = parse_standard_sheet(raw_df, cfg)
#         else:
#             continue

#         # Search
#         mask = df["Value"].str.lower().str.contains(part_lower, na=False)
#         matches = df[mask]

#         for _, row in matches.iterrows():
#             results.append({
#                 "Sheet":    sheet_name,
#                 "Value":    row["Value"],
#                 "Package":  row.get("Package", "N/A"),
#                 "Quantity": row.get("Quantity", "N/A"),
#             })

#     return results


# # ---------------------------------------------------------------------------
# # Main
# # ---------------------------------------------------------------------------
# if __name__ == "__main__":
#     print("=" * 95)
#     print(" CHINA EXTRA PART SEARCH  |  Type 'exit' or 'quit' to stop")
#     print("=" * 95)

#     while True:
#         part_to_find = input("\nEnter part name / value to search: ").strip()

#         if not part_to_find:
#             print("⚠️  Please enter a part name.")
#             continue

#         if part_to_find.lower() in ("exit", "quit"):
#             print("\nGoodbye! 👋")
#             break

#         matches = search_part(part_to_find)

#         if not matches:
#             print(f"\n❌  Part '{part_to_find}' not found in any sheet.")
#         else:
#             print(f"\n✅  Found {len(matches)} match(es) for '{part_to_find}':\n")
#             print(f"{'Sheet':<25} {'Value':<25} {'Package':<30} {'Quantity'}")
#             print("-" * 95)
#             for m in matches:
#                 print(f"{m['Sheet']:<25} {m['Value']:<25} {m['Package']:<30} {m['Quantity']}")
        
#         print("-" * 95)





"""
Part Price Lookup Tool
Searches components by name and returns price in INR.
Data sourced from Digikey BOM (USD x 90.92564347).
Exchange rate: 1 USD = ₹90.92564347
"""

# ── BOM DATA ────────────────────────────────────────────────────────────────
# Format: { "component_name": { "description": ..., "project": ...,
#            "usd_unit": ..., "inr_order_total": ..., "order_qty": ... } }

BOM = [
    # Bolt_TPS
    {"name": "0.1E",                       "desc": "R2512 ±1% 3W",                  "project": "Bolt_TPS",   "usd": 0.200,   "order_qty": 50,  "inr_total": 909.2564347},
    {"name": "0.1uf 100V 10%",             "desc": "C1206 100V 0.1UF 10%",          "project": "Bolt_TPS",   "usd": 0.027,   "order_qty": 50,  "inr_total": 122.749618685},
    {"name": "0.1uF 10%",                  "desc": "C0603 ±5% 50V 0.1UF 10%",      "project": "Bolt_TPS",   "usd": 0.009,   "order_qty": 50,  "inr_total": 40.916539562},
    {"name": "0.47uf 25V",                 "desc": "C0603 ±5% 50V=25V 0.47UF",     "project": "Bolt_TPS",   "usd": 0.017,   "order_qty": 50,  "inr_total": 77.28679695},
    {"name": "1-440054-0",                 "desc": "Connector 10-pin",              "project": "Bolt_TPS",   "usd": 0.500,   "order_qty": 10,  "inr_total": 454.62821735},
    {"name": "1k",                         "desc": "R0603 ±1%",                     "project": "Bolt_TPS",   "usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "1nF",                        "desc": "C0603 ±5% 50V",                 "project": "Bolt_TPS",   "usd": 0.011,   "order_qty": 50,  "inr_total": 50.009103909},
    {"name": "1uf 10%",                    "desc": "C0603 ±5% 50V=10%",             "project": "Bolt_TPS",   "usd": 0.025,   "order_qty": 50,  "inr_total": 113.657054338},
    {"name": "1uf",                        "desc": "C1206 100V",                    "project": "Bolt_TPS",   "usd": 0.058,   "order_qty": 50,  "inr_total": 263.684366063},
    {"name": "6-1775444-4",               "desc": "Connector",                     "project": "Bolt_TPS",   "usd": 0.0,     "order_qty": 5,   "inr_total": 0.0},
    {"name": "10k",                        "desc": "R0603 ±1%",                     "project": "Bolt_TPS",   "usd": 0.0035,  "order_qty": 200, "inr_total": 63.647950429},
    {"name": "10R",                        "desc": "R0603 ±1%",                     "project": "Bolt_TPS",   "usd": 0.0035,  "order_qty": 150, "inr_total": 47.735962822},
    {"name": "10uf 25V 10%",              "desc": "C0603 ±5% 50V=25V 10UF",       "project": "Bolt_TPS",   "usd": 0.052,   "order_qty": 50,  "inr_total": 236.406673022},
    {"name": "10uf 50V 10%",              "desc": "C1206 100V=50V 10%",            "project": "Bolt_TPS",   "usd": 0.159,   "order_qty": 150, "inr_total": 2168.57659676},
    {"name": "19.6k",                      "desc": "R0603 ±1%",                     "project": "Bolt_TPS",   "usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "47E",                        "desc": "R0603 ±1%",                     "project": "Bolt_TPS",   "usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "56k",                        "desc": "R0603 ±1%",                     "project": "Bolt_TPS",   "usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "68k",                        "desc": "R0603 ±1%",                     "project": "Bolt_TPS",   "usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "100k",                       "desc": "R0603 ±1%",                     "project": "Bolt_TPS",   "usd": 0.0035,  "order_qty": 100, "inr_total": 31.823975215},
    {"name": "100nF",                      "desc": "C0603 ±5% 50V",                 "project": "Bolt_TPS",   "usd": 0.009,   "order_qty": 100, "inr_total": 81.833079123},
    {"name": "100R",                       "desc": "R0603 ±1%",                     "project": "Bolt_TPS",   "usd": 0.0035,  "order_qty": 100, "inr_total": 31.823975215},
    {"name": "61300211121",               "desc": "Connector 2-pin",               "project": "Bolt_TPS",   "usd": 0.156,   "order_qty": 5,   "inr_total": 70.922001907},
    {"name": "B2P-VH",                    "desc": "Connector B2P3-VH",             "project": "Bolt_TPS",   "usd": 0.083,   "order_qty": 5,   "inr_total": 37.73414204},
    {"name": "CMLB136T-101MS",            "desc": "Inductor",                      "project": "Bolt_TPS",   "usd": 1.000,   "order_qty": 35,  "inr_total": 3182.39752145},
    {"name": "IAUZ40N10S5N130ATMA1",      "desc": "MOSFET N-CH 100V 40A",         "project": "Bolt_TPS",   "usd": 0.900,   "order_qty": 105, "inr_total": 8592.473307915},
    {"name": "LED_0805_GREEN",             "desc": "LED Green 0805",                "project": "Bolt_TPS",   "usd": 0.010,   "order_qty": 50,  "inr_total": 45.462821735},
    {"name": "MCP47FVB18T-20E/ST",        "desc": "Integrated Circuit 20TSSOP",    "project": "Bolt_TPS",   "usd": 7.500,   "order_qty": 5,   "inr_total": 3409.711630125},
    {"name": "SN74LVC4245APWR",           "desc": "IC Bus Transceiver",            "project": "Bolt_TPS",   "usd": 0.300,   "order_qty": 5,   "inr_total": 136.388465205},
    {"name": "STPS2H100ZF",               "desc": "Schottky Diode",                "project": "Bolt_TPS",   "usd": 0.110,   "order_qty": 35,  "inr_total": 350.06372736},
    {"name": "TPS92641PWP_NOPB",          "desc": "Integrated Circuit SOP17",      "project": "Bolt_TPS",   "usd": 2.500,   "order_qty": 35,  "inr_total": 7955.993803625},

    # Tim_TW_Rout
    {"name": "0E",                         "desc": "R0603 ±1%",                     "project": "Tim_TW_Rout","usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "0.1uF 10%",                  "desc": "C0603 ±5% 50V 0.1UF 10%",      "project": "Tim_TW_Rout","usd": 0.009,   "order_qty": 100, "inr_total": 81.833079123},
    {"name": "1k",                         "desc": "R0603 ±1%",                     "project": "Tim_TW_Rout","usd": 0.0035,  "order_qty": 100, "inr_total": 31.823975215},
    {"name": "1uf 10%",                    "desc": "C0603 ±5% 50V=10%",             "project": "Tim_TW_Rout","usd": 0.025,   "order_qty": 50,  "inr_total": 113.657054338},
    {"name": "5.1k",                       "desc": "R0603 ±1%",                     "project": "Tim_TW_Rout","usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "6pf",                        "desc": "C0603 ±5% 50V=±0.25pf",         "project": "Tim_TW_Rout","usd": 0.0065,  "order_qty": 50,  "inr_total": 29.550834128},
    {"name": "10k",                        "desc": "R0603 ±1%",                     "project": "Tim_TW_Rout","usd": 0.0035,  "order_qty": 100, "inr_total": 31.823975215},
    {"name": "10uf 25V 10%",              "desc": "C0603 ±5% 50V=25V 10UF",       "project": "Tim_TW_Rout","usd": 0.052,   "order_qty": 50,  "inr_total": 236.406673022},
    {"name": "22E",                        "desc": "R0603 ±1%",                     "project": "Tim_TW_Rout","usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "22uF 16V 10%",              "desc": "C0603 ±5% 50V=16V 10%",        "project": "Tim_TW_Rout","usd": 0.137,   "order_qty": 50,  "inr_total": 622.84065777},
    {"name": "31k",                        "desc": "R0603 ±1%",                     "project": "Tim_TW_Rout","usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "B3F-4000",                   "desc": "Switch",                        "project": "Tim_TW_Rout","usd": 0.189,   "order_qty": 10,  "inr_total": 171.849466158},
    {"name": "ESP32-C3FH4",               "desc": "Integrated Circuit QFN",        "project": "Tim_TW_Rout","usd": 1.600,   "order_qty": 10,  "inr_total": 1454.81029552},
    {"name": "FM5324GA",                   "desc": "IC SOIC8",                      "project": "Tim_TW_Rout","usd": 0.440,   "order_qty": 10,  "inr_total": 400.072831268},
    {"name": "GRM0335C1H1R2BA01J",        "desc": "Capacitor",                     "project": "Tim_TW_Rout","usd": 0.0,     "order_qty": 10,  "inr_total": 0.0},
    {"name": "GRM0335C1H1R8BA01D",        "desc": "Capacitor",                     "project": "Tim_TW_Rout","usd": 0.0021,  "order_qty": 50,  "inr_total": 9.547192564},
    {"name": "HPC6045NC-1R5M",            "desc": "Inductor",                      "project": "Tim_TW_Rout","usd": 0.0,     "order_qty": 10,  "inr_total": 0.0},
    {"name": "LED_0805_GREEN",             "desc": "LED Green 0805",                "project": "Tim_TW_Rout","usd": 0.010,   "order_qty": 100, "inr_total": 90.92564347},
    {"name": "LQP03TN2N7B02D",            "desc": "Inductor",                      "project": "Tim_TW_Rout","usd": 0.010,   "order_qty": 10,  "inr_total": 9.092564347},
    {"name": "LTST-S220KRKT",             "desc": "LED",                           "project": "Tim_TW_Rout","usd": 0.050,   "order_qty": 40,  "inr_total": 181.85128694},
    {"name": "MSS1P4-M3_89A",             "desc": "Schottky Diode",                "project": "Tim_TW_Rout","usd": 0.0677,  "order_qty": 10,  "inr_total": 61.556660629},
    {"name": "NRS4018T3R3MDGJ",           "desc": "Inductor",                      "project": "Tim_TW_Rout","usd": 0.140,   "order_qty": 10,  "inr_total": 127.295900858},
    {"name": "PTS815_SJK_250_SMTR_LFS",  "desc": "Switch",                        "project": "Tim_TW_Rout","usd": 0.650,   "order_qty": 10,  "inr_total": 591.016682555},
    {"name": "RH100-40.000-6-F-1010-TR", "desc": "Crystal/Oscillator",            "project": "Tim_TW_Rout","usd": 0.500,   "order_qty": 10,  "inr_total": 454.62821735},
    {"name": "SM712-02HTG",               "desc": "TVS Diode Bi-directional",      "project": "Tim_TW_Rout","usd": 0.180,   "order_qty": 10,  "inr_total": 163.666158246},
    {"name": "SMAJ5.0A",                  "desc": "TVS Diode Uni-directional",     "project": "Tim_TW_Rout","usd": 0.120,   "order_qty": 10,  "inr_total": 109.110772164},
    {"name": "TimoTwo",                    "desc": "Lumen Radio TimoTwo Module",    "project": "Tim_TW_Rout","usd": 0.0,     "order_qty": 10,  "inr_total": 0.0},
    {"name": "TP8485E-SR",                "desc": "Integrated Circuit SOIC8",      "project": "Tim_TW_Rout","usd": 0.080,   "order_qty": 10,  "inr_total": 72.740514776},
    {"name": "TPS562207DRLR",             "desc": "Integrated Circuit SOT-23-6",   "project": "Tim_TW_Rout","usd": 0.070,   "order_qty": 10,  "inr_total": 63.647950429},
    {"name": "U.FL-R-SMT-1(01)",          "desc": "Coaxial Connector U.FL",        "project": "Tim_TW_Rout","usd": 0.450,   "order_qty": 10,  "inr_total": 409.165395615},
    {"name": "UJ20-C-H-G-SMT-1-P16-TR",  "desc": "Connector Type C",              "project": "Tim_TW_Rout","usd": 1.800,   "order_qty": 10,  "inr_total": 1636.66158246},
    {"name": "WP113IDT",                   "desc": "LED",                           "project": "Tim_TW_Rout","usd": 0.050,   "order_qty": 100, "inr_total": 454.62821735},

    # BLT_GUI
    {"name": "0E",                         "desc": "R0603 ±1%",                     "project": "BLT_GUI",    "usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "0.022uf",                    "desc": "C0603 ±5% 50V",                 "project": "BLT_GUI",    "usd": 0.013,   "order_qty": 5,   "inr_total": 5.910166826},
    {"name": "0.1uF 10%",                  "desc": "C0603 ±5% 50V 0.1UF 10%",      "project": "BLT_GUI",    "usd": 0.009,   "order_qty": 150, "inr_total": 122.749618685},
    {"name": "1-440054-0",                 "desc": "Connector 10-pin",              "project": "BLT_GUI",    "usd": 0.500,   "order_qty": 30,  "inr_total": 1363.88465205},
    {"name": "1k",                         "desc": "R0603 ±1%",                     "project": "BLT_GUI",    "usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "1uf 10%",                    "desc": "C0603 ±5% 50V=10%",             "project": "BLT_GUI",    "usd": 0.025,   "order_qty": 100, "inr_total": 227.314108675},
    {"name": "1.5k",                       "desc": "R0603 ±1%",                     "project": "BLT_GUI",    "usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "2.2uf 10%",                  "desc": "C0603 ±5% 50V=10%",             "project": "BLT_GUI",    "usd": 0.050,   "order_qty": 50,  "inr_total": 227.314108675},
    {"name": "4.5k",                       "desc": "R0603 ±1%",                     "project": "BLT_GUI",    "usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "4.75k",                      "desc": "R0603 ±1%",                     "project": "BLT_GUI",    "usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "5.6k",                       "desc": "R0603 ±1%",                     "project": "BLT_GUI",    "usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "6.2k",                       "desc": "R0603 ±1%",                     "project": "BLT_GUI",    "usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "6.8nf",                      "desc": "C0603 ±5% 50V",                 "project": "BLT_GUI",    "usd": 0.012,   "order_qty": 50,  "inr_total": 54.555386082},
    {"name": "8pf",                        "desc": "C0603 ±5% 50V",                 "project": "BLT_GUI",    "usd": 0.009,   "order_qty": 50,  "inr_total": 40.916539562},
    {"name": "10E",                        "desc": "R0603 ±1%",                     "project": "BLT_GUI",    "usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "10k",                        "desc": "R0603 ±1%",                     "project": "BLT_GUI",    "usd": 0.0035,  "order_qty": 150, "inr_total": 47.735962822},
    {"name": "10nf",                       "desc": "C0603 ±5% 50V",                 "project": "BLT_GUI",    "usd": 0.009,   "order_qty": 50,  "inr_total": 40.916539562},
    {"name": "10uf 25V 10%",              "desc": "C0603 ±5% 50V=25V 10UF",       "project": "BLT_GUI",    "usd": 0.052,   "order_qty": 100, "inr_total": 472.813346044},
    {"name": "10uf 50V 10%",              "desc": "C1206 ±5% 50V=50V 10%",        "project": "BLT_GUI",    "usd": 0.160,   "order_qty": 50,  "inr_total": 727.40514776},
    {"name": "12.1k",                      "desc": "R0603 ±1%",                     "project": "BLT_GUI",    "usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "13k",                        "desc": "R0603 ±1%",                     "project": "BLT_GUI",    "usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "47k",                        "desc": "R0603 ±1%",                     "project": "BLT_GUI",    "usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "49.9E",                      "desc": "R0603 ±1%",                     "project": "BLT_GUI",    "usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "100k",                       "desc": "R0603 ±1%",                     "project": "BLT_GUI",    "usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "330E",                       "desc": "R0603 ±1%",                     "project": "BLT_GUI",    "usd": 0.0035,  "order_qty": 50,  "inr_total": 15.911987607},
    {"name": "470pf",                      "desc": "C0603 ±5% 50V",                 "project": "BLT_GUI",    "usd": 0.012,   "order_qty": 50,  "inr_total": 54.555386082},
    {"name": "440054-3",                   "desc": "Connector 3-pin",               "project": "BLT_GUI",    "usd": 0.100,   "order_qty": 10,  "inr_total": 90.92564347},
    {"name": "440054-6",                   "desc": "Connector 6-pin",               "project": "BLT_GUI",    "usd": 0.100,   "order_qty": 10,  "inr_total": 90.92564347},
    {"name": "0874371273",                 "desc": "Connector",                     "project": "BLT_GUI",    "usd": 1.700,   "order_qty": 5,   "inr_total": 772.867969495},
    {"name": "885342208022",               "desc": "Capacitor special",             "project": "BLT_GUI",    "usd": 0.0,     "order_qty": 5,   "inr_total": 0.0},
    {"name": "B2P-VH",                    "desc": "Connector B2P3-VH",             "project": "BLT_GUI",    "usd": 0.100,   "order_qty": 5,   "inr_total": 45.462821735},
    {"name": "BD9G341AEFJ-E2",            "desc": "IC LED Driver SOIC8",           "project": "BLT_GUI",    "usd": 1.500,   "order_qty": 5,   "inr_total": 681.942326025},
    {"name": "CA-IS3082WX",               "desc": "Isolated RS-485 SOIC-16",       "project": "BLT_GUI",    "usd": 0.9141,  "order_qty": 5,   "inr_total": 415.57565348},
    {"name": "CDBA340-G",                  "desc": "Schottky Diode",                "project": "BLT_GUI",    "usd": 0.250,   "order_qty": 15,  "inr_total": 340.971163013},
    {"name": "CME0305S3C",                 "desc": "Power Supply Module",           "project": "BLT_GUI",    "usd": 6.000,   "order_qty": 5,   "inr_total": 2727.7693041},
    {"name": "CPDT-5V0U-HF",              "desc": "Diode SOT-23",                  "project": "BLT_GUI",    "usd": 0.120,   "order_qty": 5,   "inr_total": 54.555386082},
    {"name": "ECS-3225MV-500-BN-TR",      "desc": "Crystal/Oscillator 500MHz",     "project": "BLT_GUI",    "usd": 1.000,   "order_qty": 5,   "inr_total": 454.62821735},
    {"name": "ESP32-WROOM-32UE",          "desc": "Integrated Circuit ESP32 16MB", "project": "BLT_GUI",    "usd": 3.300,   "order_qty": 5,   "inr_total": 1500.273117255},
    {"name": "J0011D01BNL",               "desc": "RJ45 Connector",                "project": "BLT_GUI",    "usd": 1.950,   "order_qty": 5,   "inr_total": 886.525023833},
    {"name": "LAN8720A-CP-ABC",           "desc": "Ethernet PHY IC QFN",           "project": "BLT_GUI",    "usd": 2.200,   "order_qty": 5,   "inr_total": 1000.18207817},
    {"name": "LED_0805_BLUE",              "desc": "LED Blue 0805",                 "project": "BLT_GUI",    "usd": 0.010,   "order_qty": 50,  "inr_total": 45.462821735},
    {"name": "LED_0805_GREEN",             "desc": "LED Green 0805",                "project": "BLT_GUI",    "usd": 0.010,   "order_qty": 50,  "inr_total": 45.462821735},
    {"name": "MPZ2012S601ATD25",          "desc": "Ferrite Bead",                  "project": "BLT_GUI",    "usd": 0.300,   "order_qty": 5,   "inr_total": 136.388465205},
    {"name": "MSS1P4-M3_89A",             "desc": "Schottky Diode",                "project": "BLT_GUI",    "usd": 0.0677,  "order_qty": 5,   "inr_total": 30.778330315},
    {"name": "NRS8040T330MJGK",           "desc": "Inductor 33µH",                 "project": "BLT_GUI",    "usd": 0.700,   "order_qty": 5,   "inr_total": 318.239752145},
    {"name": "PTS815_SJK_250_SMTR_LFS",  "desc": "Switch SMD",                    "project": "BLT_GUI",    "usd": 0.650,   "order_qty": 10,  "inr_total": 591.016682555},
    {"name": "RH100-24.000-8-1015-EXT",  "desc": "Crystal 24MHz",                 "project": "BLT_GUI",    "usd": 0.500,   "order_qty": 5,   "inr_total": 227.314108675},
    {"name": "SMAJ5.0A",                  "desc": "TVS Diode Uni-directional",     "project": "BLT_GUI",    "usd": 0.120,   "order_qty": 5,   "inr_total": 54.555386082},
    {"name": "SMD1206B075TF/13.2",        "desc": "Fuse 0.75A 1206",               "project": "BLT_GUI",    "usd": 0.100,   "order_qty": 5,   "inr_total": 45.462821735},
    {"name": "SN74LVC4245APWR",           "desc": "IC Bus Transceiver SOP24",      "project": "BLT_GUI",    "usd": 0.300,   "order_qty": 25,  "inr_total": 681.942326025},
    {"name": "STM32H747BGT6",             "desc": "MCU STM32H7 208-QFP",          "project": "BLT_GUI",    "usd": 28.000,  "order_qty": 5,   "inr_total": 12729.5900858},
    {"name": "TimoTwo",                    "desc": "Lumen Radio TimoTwo Module",    "project": "BLT_GUI",    "usd": 0.0,     "order_qty": 5,   "inr_total": 0.0},
    {"name": "TS5A23157DGSR",             "desc": "Analog Switch IC SOP10",        "project": "BLT_GUI",    "usd": 0.500,   "order_qty": 5,   "inr_total": 227.314108675},
    {"name": "TZMC5V6-GS18",              "desc": "Zener Diode 5.6V",              "project": "BLT_GUI",    "usd": 0.200,   "order_qty": 100, "inr_total": 1818.5128694},
    {"name": "XF2M-4215-1A",              "desc": "FFC Connector 42-pin",          "project": "BLT_GUI",    "usd": 1.800,   "order_qty": 5,   "inr_total": 818.33079123},
    {"name": "ZXMN3A01FTA",               "desc": "MOSFET N-Ch SOT-23",            "project": "BLT_GUI",    "usd": 0.200,   "order_qty": 5,   "inr_total": 90.92564347},
]

EXCHANGE_RATE = 90.92564347


def search_part(query: str):
    """
    Search for a part by name (case-insensitive substring match).
    Returns list of matching entries with unit price in INR.
    """
    q = query.strip().lower()
    results = []
    for item in BOM:
        if q in item["name"].lower() or q in item["desc"].lower():
            unit_inr = (item["inr_total"] / item["order_qty"]) if item["order_qty"] > 0 else 0.0
            results.append({
                **item,
                "unit_inr": unit_inr,
            })
    return results


def print_results(query, results):
    if not results:
        print(f"\n  ❌  No match found for '{query}'")
        return

    print(f"\n  ✅  {len(results)} result(s) for '{query}':\n")
    header = f"  {'Component':<35} {'Description':<35} {'Project':<14} {'USD/unit':>9} {'INR/unit':>10} {'Order Qty':>10} {'INR Total':>12}"
    print(header)
    print("  " + "─" * (len(header) - 2))
    for r in results:
        print(
            f"  {r['name']:<35} {r['desc']:<35} {r['project']:<14}"
            f"  ${r['usd']:>8.4f}  ₹{r['unit_inr']:>9.4f}  {r['order_qty']:>9}  ₹{r['inr_total']:>11.4f}"
        )


# ── MAIN LOOP ────────────────────────────────────────────────────────────────
if __name__ == "__main__":
    print("=" * 110)
    print(f"  DIGIKEY BOM PRICE LOOKUP  |  Rate: 1 USD = ₹{EXCHANGE_RATE}  |  Type 'exit' to quit")
    print("=" * 110)

    while True:
        try:
            query = input("\n  Enter component name to search: ").strip()
        except (EOFError, KeyboardInterrupt):
            print("\n\n  Goodbye! 👋")
            break

        if not query:
            print("  ⚠️  Please enter a component name.")
            continue

        if query.lower() in ("exit", "quit", "q"):
            print("\n  Goodbye! 👋")
            break

        results = search_part(query)
        print_results(query, results)
        print()