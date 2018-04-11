type product = string;
type timestamp = ISOString(string);

type taxLot = {
  product,
  size: float,
  buyPrice: float,
  dateAcquired: timestamp,
};

type sale = {
  taxLot,
  sellPrice: float,
  dateSold: timestamp,
};

type sellResult =
  | InvalidSize
  | InsufficientTaxLots
  | Success(list(sale), list(taxLot));

let makeSale = (size: float, sellPrice: float, dateSold: timestamp, lot: taxLot) => {
  let filled = size <= lot.size ? size : lot.size;
  let sale = {
    taxLot: { ...lot, size: filled },
    sellPrice,
    dateSold,
  };
  let unfilledSize = size -. filled;
  let leftoverSizeOnLot = lot.size -. filled;
  let leftoverLot = leftoverSizeOnLot > 0. ? Some({ ...lot, size: leftoverSizeOnLot }) : None;
  (sale, unfilledSize, leftoverLot)
};

let rec recSell = (size, sellPrice, dateSold, lots, salesSoFar) => {
  if (size == 0.) {
    Success(salesSoFar, lots)
  } else if (size < 0.) {
    InvalidSize
  } else switch (lots) {
    | [] => InsufficientTaxLots
    | [first, ...rest] => {
      let (sale, size, leftoverLot) = makeSale(size, sellPrice, dateSold, first);
      let sales = salesSoFar @ [sale];
      let lots = switch(leftoverLot) {
      | None => rest
      | Some(leftoverLot) => [leftoverLot, ...rest]
      };
      recSell(size, sellPrice, dateSold, lots, sales)
    }
  }
};

let sell = (size: float, sellPrice: float, dateSold: timestamp, lots: list(taxLot)): sellResult =>
  recSell(size, sellPrice, dateSold, lots, []);
