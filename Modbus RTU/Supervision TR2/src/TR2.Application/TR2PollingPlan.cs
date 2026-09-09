namespace TR2.Application;

public enum TR2RegisterBlock
{
    B0,
    B1,
    B2,
    B3,
    B4,
    B5,
    B6,
    B7
}

public static class TR2PollingPlan
{
    public static PollingGroup GetGroup(TR2RegisterBlock block) => block switch
    {
        TR2RegisterBlock.B0 => PollingGroup.Static,
        TR2RegisterBlock.B1 or TR2RegisterBlock.B3 or TR2RegisterBlock.B5 => PollingGroup.Fast,
        TR2RegisterBlock.B2 or TR2RegisterBlock.B7 => PollingGroup.Medium,
        TR2RegisterBlock.B4 or TR2RegisterBlock.B6 => PollingGroup.Slow,
        _ => throw new ArgumentOutOfRangeException(nameof(block))
    };

    public static IReadOnlyList<TR2RegisterBlock> PostReconnectRefreshBlocks { get; } =
        [
            TR2RegisterBlock.B1,
            TR2RegisterBlock.B2,
            TR2RegisterBlock.B3,
            TR2RegisterBlock.B4,
            TR2RegisterBlock.B5,
            TR2RegisterBlock.B6,
            TR2RegisterBlock.B7
        ];
}
