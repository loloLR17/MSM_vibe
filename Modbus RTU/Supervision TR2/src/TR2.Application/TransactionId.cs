namespace TR2.Application;

public readonly record struct TransactionId
{
    public TransactionId(ushort value)
    {
        if (value == 0)
        {
            throw new ArgumentOutOfRangeException(nameof(value), "transaction_id must be in 1..65535.");
        }

        Value = value;
    }

    public ushort Value { get; }
}
